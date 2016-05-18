/*
 * FFM (ffserver live feed) muxer
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

static void flush_packet(AVFormatContext *s)
{
    FFMContext *ffm = s->priv_data;
    int fill_size, h;
    AVIOContext *pb = s->pb;

    fill_size = ffm->packet_end - ffm->packet_ptr;
    memset(ffm->packet_ptr, 0, fill_size);

    /*if (avio_tell(pb) % ffm->packet_size)
        av_abort();*/

    /* put header */
    avio_wb16(pb, PACKET_ID);
    avio_wb16(pb, fill_size);
    avio_wb64(pb, ffm->dts);
    h = ffm->frame_offset;
    if (ffm->first_packet)
        h |= 0x8000;
    avio_wb16(pb, h);
    avio_write(pb, ffm->packet, ffm->packet_end - ffm->packet);
    avio_flush(pb);

    /* prepare next packet */
    ffm->frame_offset = 0; /* no key frame */
    ffm->packet_ptr = ffm->packet;
    ffm->first_packet = 0;
}

/* 'first' is true if first data of a frame */
static void ffm_write_data(AVFormatContext *s,
                           const uint8_t *buf, int size,
                           int64_t dts, int header)
{
    FFMContext *ffm = s->priv_data;
    int len;

    if (header && ffm->frame_offset == 0) {
        ffm->frame_offset = ffm->packet_ptr - ffm->packet + FFM_HEADER_SIZE;
        ffm->dts = dts;
    }

    /* write as many packets as needed */
    while (size > 0) {
        len = ffm->packet_end - ffm->packet_ptr;
        if (len > size)
            len = size;
        memcpy(ffm->packet_ptr, buf, len);

        ffm->packet_ptr += len;
        buf += len;
        size -= len;
        if (ffm->packet_ptr >= ffm->packet_end)
            flush_packet(s);
    }
}

int ffm_write_header(AVFormatContext *s)
{
    FFMContext *ffm = s->priv_data;
    AVIOContext *pb = s->pb;
    int bit_rate, i;

    ffm->packet_size = FFM_PACKET_SIZE;

    /* header */
    avio_wl32(pb, MKTAG('F', 'F', 'M', '1'));
    avio_wb32(pb, ffm->packet_size);
    avio_wb64(pb, 0); /* current write position */

    avio_wb32(pb, 1);
    bit_rate = 0;
    avio_wb32(pb, bit_rate);

    avio_write(pb, s->stream0_data, FFM_STREAM_HEADER_SIZE);

    /* flush until end of block reached */
    while ((avio_tell(pb) % ffm->packet_size) != 0)
        avio_w8(pb, 0);

    avio_flush(pb);

    /* init packet mux */
    ffm->packet_ptr = ffm->packet;
    ffm->packet_end = ffm->packet + ffm->packet_size - FFM_HEADER_SIZE;
    //assert(ffm->packet_end >= ffm->packet);
    ffm->frame_offset = 0;
    ffm->dts = 0;
    ffm->first_packet = 1;

    return 0;
}

int ffm_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    int64_t dts;
    uint8_t header[FRAME_HEADER_SIZE+4];
    int header_size = FRAME_HEADER_SIZE;

    dts = pkt->dts;
    /* packet size & key_frame */
    header[0] = pkt->stream_index;
    header[1] = 0;
    if (pkt->flags & AV_PKT_FLAG_KEY)
        header[1] |= FLAG_KEY_FRAME;
    AV_WB24(header+2, pkt->size);
    AV_WB24(header+5, pkt->duration);
    AV_WB64(header+8, pkt->pts);
    if (pkt->pts != pkt->dts) {
        header[1] |= FLAG_DTS;
        AV_WB32(header+16, pkt->pts - pkt->dts);
        header_size += 4;
    }
    ffm_write_data(s, header, header_size, dts, 1);
    ffm_write_data(s, pkt->data, pkt->size, dts, 0);

    return 0;
}

/*static*/ int ffm_write_trailer(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    FFMContext *ffm = s->priv_data;

    /* flush packets */
    if (ffm->packet_ptr > ffm->packet)
        flush_packet(s);

    avio_flush(pb);

    return 0;
}

#if 0
AVOutputFormat ff_ffm_muxer = {
    .name              = "ffm",
    .long_name         = NULL_IF_CONFIG_SMALL("FFM (FFserver live feed) format"),
    .extensions        = "ffm",
    .priv_data_size    = sizeof(FFMContext),
    .audio_codec       = CODEC_ID_MP2,
    .video_codec       = CODEC_ID_MPEG1VIDEO,
    .write_header      = ffm_write_header,
    .write_packet      = ffm_write_packet,
    .write_trailer     = ffm_write_trailer,
};
#endif
