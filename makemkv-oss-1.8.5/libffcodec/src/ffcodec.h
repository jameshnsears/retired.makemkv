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
#ifndef FFCODEC_H
#define FFCODEC_H

#include <stdint.h>
#include "mlp_parser.h"
#include "mpegaudiodecheader.h"
#include "mpegaudiomisc.h"
#include "ffm.h"

typedef struct FFCFFMContextR
{
    AVFormatContext     fmt;
    AVIOContext         avio;
    FFMContext          ffm;
} FFCFFMContextR;

typedef struct FFCFFMContextW
{
    FFCFFMContextR      ctx;
    uint8_t             avio_buffer[8*FFM_PACKET_SIZE];
} FFCFFMContextW;


#ifdef __cplusplus
extern "C" {
#endif

int ffcdecl ffcodec_ff_mlp_read_major_sync(MLPHeaderInfo *mh, const uint8_t* data, unsigned int size);
int ffcdecl ffcodec_ff_mlp_get_audio_info(const MLPHeaderInfo *mh,AVAudioInfo* avctx);


int ffcdecl ffcodec_ff_mpegaudio_decode_header(MPADecodeHeader *s, const uint8_t* data, unsigned int size);
int ffcdecl ffcodec_ff_mpa_check_header( const uint8_t* data, unsigned int size);

int ffcdecl ffcodec_ffm_init_context_r(FFCFFMContextR *s,uintptr_t fd,AVIOReadProc read);
int ffcdecl ffcodec_ffm_init_context_w(FFCFFMContextW *s,uintptr_t fd,AVIOWriteProc write);
int ffcdecl ffcodec_ffm_read_header(FFCFFMContextR *s);
int ffcdecl ffcodec_ffm_read_packet_header(FFCFFMContextR *s, uint32_t *size);
int ffcdecl ffcodec_ffm_read_packet_data(FFCFFMContextR *s, AVPacket *pkt);
int ffcdecl ffcodec_ffm_write_header(FFCFFMContextW *s);
int ffcdecl ffcodec_ffm_write_packet(FFCFFMContextW *s, AVPacket *pkt);
int ffcdecl ffcodec_ffm_write_trailer(FFCFFMContextW *s);

#ifdef __cplusplus
}
#endif

#endif /* FFCODEC_H */

