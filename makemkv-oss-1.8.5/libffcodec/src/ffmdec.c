/*
 * FFM (ffserver live feed) demuxer
 * Copyright (c) 2001 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "ffm.h"
#if CONFIG_FFSERVER
#include <unistd.h>

int64_t ffm_read_write_index(int fd)
{
    uint8_t buf[8];

    lseek(fd, 8, SEEK_SET);
    if (read(fd, buf, 8) != 8)
        return AVERROR(EIO);
    return AV_RB64(buf);
}

int ffm_write_write_index(int fd, int64_t pos)
{
    uint8_t buf[8];
    int i;

    for(i=0;i<8;i++)
        buf[i] = (pos >> (56 - i * 8)) & 0xff;
    lseek(fd, 8, SEEK_SET);
    if (write(fd, buf, 8) != 8)
        return AVERROR(EIO);
    return 8;
}

void ffm_set_write_index(AVFormatContext *s, int64_t pos, int64_t file_size)
{
    FFMContext *ffm = s->priv_data;
    ffm->write_index = pos;
    ffm->file_size = file_size;
}
#endif // CONFIG_FFSERVER

static int ffm_is_avail_data(AVFormatContext *s, int size)
{
    return 1;
}

static int ffm_resync(AVFormatContext *s, int state)
{
    return -1;
}

/* first is true if we read the frame header */
static int ffm_read_data(AVFormatContext *s,
                         uint8_t *buf, int size, int header)
{
    FFMContext *ffm = s->priv_data;
    AVIOContext *pb = s->pb;
    int len, fill_size, size1, frame_offset, id;

    size1 = size;
    while (size > 0) {
    redo:
        len = ffm->packet_end - ffm->packet_ptr;
        if (len < 0)
            return -1;
        if (len > size)
            len = size;
        if (len == 0) {
            /*if (avio_tell(pb) == ffm->file_size)
                avio_seek(pb, ffm->packet_size, SEEK_SET);
    retry_read:
              if (pb->buffer_size != ffm->packet_size) {
                int64_t tell = avio_tell(pb);
                url_setbufsize(pb, ffm->packet_size);
                avio_seek(pb, tell, SEEK_SET);
            }*/
            id = avio_rb16(pb); /* PACKET_ID */
            if (id != PACKET_ID)
                if (ffm_resync(s, id) < 0)
                    return -1;
            fill_size = avio_rb16(pb);
            ffm->dts = avio_rb64(pb);
            frame_offset = avio_rb16(pb);
            if (avio_read(pb, ffm->packet, ffm->packet_size - FFM_HEADER_SIZE) < 0)
                return -1;
            ffm->packet_end = ffm->packet + (ffm->packet_size - FFM_HEADER_SIZE - fill_size);
            if (ffm->packet_end < ffm->packet || frame_offset < 0)
                return -1;
            /* if first packet or resynchronization packet, we must
               handle it specifically */
            if (ffm->first_packet || (frame_offset & 0x8000)) {
                if (!frame_offset) {
                    /* This packet has no frame headers in it */
                    /*if (avio_tell(pb) >= ffm->packet_size * 3) {
                        avio_seek(pb, -ffm->packet_size * 2, SEEK_CUR);
                        goto retry_read;
                    }*/
                    /* This is bad, we cannot find a valid frame header */
                    return 0;
                }
                ffm->first_packet = 0;
                if ((frame_offset & 0x7fff) < FFM_HEADER_SIZE)
                    return -1;
                ffm->packet_ptr = ffm->packet + (frame_offset & 0x7fff) - FFM_HEADER_SIZE;
                if (!header)
                    break;
            } else {
                ffm->packet_ptr = ffm->packet;
            }
            goto redo;
        }
        memcpy(buf, ffm->packet_ptr, len);
        buf += len;
        ffm->packet_ptr += len;
        size -= len;
        header = 0;
    }
    return size1 - size;
}

int ffm_read_header(AVFormatContext *s)
{
    FFMContext *ffm = s->priv_data;
    AVIOContext *pb = s->pb;
    int i, nb_streams;
    uint32_t tag;

    /* header */
    tag = avio_rl32(pb);
    if (tag != MKTAG('F', 'F', 'M', '1'))
        goto fail;
    ffm->packet_size = avio_rb32(pb);
    if (ffm->packet_size != FFM_PACKET_SIZE)
        goto fail;
    ffm->write_index = avio_rb64(pb);
    /* get also filesize */
    /*if (pb->seekable) {
        ffm->file_size = avio_size(pb);
        if (ffm->write_index)
            adjust_write_index(s);
    } else*/ {
        ffm->file_size = (UINT64_C(1) << 63) - 1;
    }

    nb_streams = avio_rb32(pb);
    avio_rb32(pb); /* total bitrate */

    if (nb_streams !=1 )
        goto fail;

    if (avio_read(pb, s->stream0_data, FFM_STREAM_HEADER_SIZE) < 0)
        return -1;

    /* get until end of block reached */
    while ((avio_tell(pb) % ffm->packet_size) != 0)
        avio_r8(pb);

    /* init packet demux */
    ffm->packet_ptr = ffm->packet;
    ffm->packet_end = ffm->packet;
    ffm->frame_offset = 0;
    ffm->dts = 0;
    ffm->read_state = READ_HEADER;
    ffm->first_packet = 1;
    return 0;
 fail:
    //ffm_close(s);
    return -1;
}

/* return < 0 if eof */
int ffm_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    int size;
    FFMContext *ffm = s->priv_data;
    int duration, ret;

    switch(ffm->read_state) {
    case READ_HEADER:
        if ((ret = ffm_is_avail_data(s, FRAME_HEADER_SIZE+4)) < 0)
            return ret;

        /*av_dlog(s, "pos=%08"PRIx64" spos=%"PRIx64", write_index=%"PRIx64" size=%"PRIx64"\n",
               avio_tell(s->pb), s->pb->pos, ffm->write_index, ffm->file_size);*/
        if (ffm_read_data(s, ffm->header, FRAME_HEADER_SIZE, 1) !=
            FRAME_HEADER_SIZE)
            return -1;
        if (ffm->header[1] & FLAG_DTS)
            if (ffm_read_data(s, ffm->header+16, 4, 1) != 4)
                return -1;
        ffm->read_state = READ_DATA;
        return 0;
    case READ_DATA:
        size = AV_RB24(ffm->header + 2);
        if ((ret = ffm_is_avail_data(s, size)) < 0)
            return ret;

        duration = AV_RB24(ffm->header + 5);

        //av_new_packet(pkt, size);
        pkt->stream_index = ffm->header[0];
        if ((unsigned)pkt->stream_index != 0) {
            //av_log(s, AV_LOG_ERROR, "invalid stream index %d\n", pkt->stream_index);
            //av_free_packet(pkt);
            ffm->read_state = READ_HEADER;
            return -1;
        }
        pkt->pos = avio_tell(s->pb);
        if (ffm->header[1] & FLAG_KEY_FRAME)
            pkt->flags |= AV_PKT_FLAG_KEY;

        ffm->read_state = READ_HEADER;
        if (ffm_read_data(s, pkt->data, size, 0) != size) {
            /* bad case: desynchronized packet. we cancel all the packet loading */
            //av_free_packet(pkt);
            return -1;
        }
        pkt->pts = AV_RB64(ffm->header+8);
        if (ffm->header[1] & FLAG_DTS)
            pkt->dts = pkt->pts - AV_RB32(ffm->header+16);
        else
            pkt->dts = pkt->pts;
        pkt->duration = duration;
        break;
    }
    return 0;
}

