/*
    libMakeMKV - MKV multiplexer library

    Copyright (C) 2007-2013 GuinpinSoft inc <libmkv@makemkv.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
#include "ffcodec.h"
#include "get_bits.h"
#include "mlp.h"
#include "mpegaudiodecheader.h"
#include <stdlib.h>

int ffm_read_header(AVFormatContext *s);
int ffm_read_packet(AVFormatContext *s, AVPacket *pkt);
int ffm_write_header(AVFormatContext *s);
int ffm_write_packet(AVFormatContext *s, AVPacket *pkt);
int ffm_write_trailer(AVFormatContext *s);

int ffcdecl ffcodec_ff_mlp_read_major_sync(MLPHeaderInfo *mh, const uint8_t* data, unsigned int size)
{
    GetBitContext   gb;

    init_get_bits(&gb,data,size*8);

    ff_mlp_init_crc();

    return ff_mlp_read_major_sync(NULL,mh,&gb);
}

int ffcdecl ffcodec_ff_mpegaudio_decode_header(MPADecodeHeader *s, const uint8_t* data, unsigned int size)
{
    uint32_t header;

    if (size<4) return -2;

    header = AV_RB32(data);

    return avpriv_mpegaudio_decode_header(s,header);
}

int ffcdecl ffcodec_ff_mpa_check_header( const uint8_t* data, unsigned int size)
{
    uint32_t header;

    if (size<4) return -2;

    header = AV_RB32(data);

    return ff_mpa_check_header(header);
}

int ffcdecl ffcodec_ffm_init_context_r(FFCFFMContextR *s,uintptr_t fd,AVIOReadProc read)
{
    memset(s,0,sizeof(*s));
    s->avio.fd=fd;
    s->avio.read=read;
    s->fmt.pb=&s->avio;
    s->fmt.priv_data=&s->ffm;
    return 0;
}

int ffcdecl ffcodec_ffm_init_context_w(FFCFFMContextW *s,uintptr_t fd,AVIOWriteProc write)
{
    ffcodec_ffm_init_context_r(&s->ctx,fd,NULL);
    s->ctx.avio.write=write;
    s->ctx.avio.buffer=s->avio_buffer;
    s->ctx.avio.buffer_max_size=8*FFM_PACKET_SIZE;
    return 0;
}


int ffcdecl ffcodec_ffm_read_header(FFCFFMContextR *s)
{
    return ffm_read_header(&s->fmt);
}

int ffcdecl ffcodec_ffm_read_packet_header(FFCFFMContextR *s, uint32_t *size)
{
    int err;

    if (s->ffm.read_state!=READ_HEADER) return -1;
    if ((err=ffm_read_packet(&s->fmt,NULL))<0) return err;

    *size = AV_RB24(s->ffm.header + 2);

    return 0;
}

int ffcdecl ffcodec_ffm_read_packet_data(FFCFFMContextR *s, AVPacket *pkt)
{
    if (s->ffm.read_state!=READ_DATA) return -1;

    return ffm_read_packet(&s->fmt,pkt);
}

int ffcdecl ffcodec_ffm_write_header(FFCFFMContextW *s)
{
    return ffm_write_header(&s->ctx.fmt);
}

int ffcdecl ffcodec_ffm_write_packet(FFCFFMContextW *s, AVPacket *pkt)
{
    return ffm_write_packet(&s->ctx.fmt,pkt);
}

int ffcdecl ffcodec_ffm_write_trailer(FFCFFMContextW *s)
{
    return ffm_write_trailer(&s->ctx.fmt);
}

void avio_flush(AVIOContext *s)
{
    if (s->buffer_size)
    {
        (*s->write)(s->fd,s->buffer,s->buffer_size);
        s->buffer_size = 0;
    }
}

int64_t avio_tell(AVIOContext *s)
{
    return s->pos;
}

int avio_read(AVIOContext *s, unsigned char *buf, int size)
{
    int err = (*s->read)(s->fd,buf,size);
    if (err>=0)
    {
        s->pos += err;
    }
    return err;
}

void avio_write(AVIOContext *s, const unsigned char *buf, int size)
{
    int err;

    if ((size+s->buffer_size)<=s->buffer_max_size)
    {
        memcpy(s->buffer+s->buffer_size,buf,size);
        s->buffer_size += size;
        s->pos += size;
    } else {
        avio_flush(s);

        err = (*s->write)(s->fd,buf,size);
        if (err>=0)
        {
            s->pos += err;
        }
    }
}

int avio_r8(AVIOContext *s)
{
    unsigned char buf=0;

    avio_read(s,&buf,1);

    return buf;
}

void avio_w8(AVIOContext *s, int b)
{
    unsigned char buf=(unsigned char)b;
    avio_write(s,&buf,1);
}

