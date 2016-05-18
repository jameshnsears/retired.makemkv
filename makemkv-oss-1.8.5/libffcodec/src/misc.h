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
#ifndef FFCODEC_MISC_H
#define FFCODEC_MISC_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_HARDCODED_TABLES     1
#define CONFIG_SMALL                1
#define CONFIG_FFSERVER             0
#define ALT_BITSTREAM_READER        1

#define av_pure
#define av_cold
#define av_unused

#ifdef _MSC_VER
#define inline              __inline
#define av_always_inline    __inline
#define ffcdecl             __cdecl
#else
#define inline              __inline__
#define av_always_inline    __inline__
#define ffcdecl
#endif

static uint32_t av_bswap32(uint32_t i)
{
    uint32_t j;
    j =  (i << 24);
    j |= (i <<  8) & 0x00FF0000;
    j |= (i >>  8) & 0x0000FF00;
    j |= (i >> 24);
    return j;
}

#define AVERROR(x)          -4

typedef int (ffcdecl *AVIOReadProc)(uintptr_t fd, unsigned char *buf, unsigned int size);
typedef int (ffcdecl *AVIOWriteProc)(uintptr_t fd, const unsigned char *buf, unsigned int size);

typedef struct AVIOContext
{
    uintptr_t       fd;
    uint64_t        pos;
    AVIOReadProc    read;
    AVIOWriteProc   write;
    uint8_t*        buffer;
    int             buffer_size;
    int             buffer_max_size;
} AVIOContext;

#define FFM_STREAM_HEADER_SIZE     0x0100 ///< Stream header size

typedef struct AVFormatContext
{
    void*           priv_data;
    AVIOContext*    pb;
    uint8_t         stream0_data[FFM_STREAM_HEADER_SIZE];
} AVFormatContext;

typedef struct AVPacket
{
    uint8_t*        data;
    uint32_t        size;
    uint64_t        pos;
    uint32_t        flags;
    uint8_t         stream_index;
    uint64_t        pts;
    uint64_t        dts;
    uint32_t        duration;
} AVPacket;

typedef struct AVAudioInfo
{
    int             channels;
    uint64_t        channel_layout;
} AVAudioInfo;

#include "ffmisc.h"


#endif /* FFCODEC_MISC_H */

