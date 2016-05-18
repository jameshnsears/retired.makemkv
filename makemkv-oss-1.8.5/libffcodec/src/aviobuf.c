/*
 * buffered I/O
 * Copyright (c) 2000,2001 Fabrice Bellard
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

#include "misc.h"

void avio_wl32(AVIOContext *s, unsigned int val)
{
    avio_w8(s, val);
    avio_w8(s, val >> 8);
    avio_w8(s, val >> 16);
    avio_w8(s, val >> 24);
}

void avio_wb32(AVIOContext *s, unsigned int val)
{
    avio_w8(s, val >> 24);
    avio_w8(s, val >> 16);
    avio_w8(s, val >> 8);
    avio_w8(s, val);
}

#define GET(name, type) \
    type get_be ##name(AVIOContext *s) \
{\
    return avio_rb ##name(s);\
}\
    type get_le ##name(AVIOContext *s) \
{\
    return avio_rl ##name(s);\
}

GET(16, unsigned int)
GET(24, unsigned int)
GET(32, unsigned int)
GET(64, uint64_t)

#undef GET

#define PUT(name, type ) \
    void put_le ##name(AVIOContext *s, type val)\
{\
        avio_wl ##name(s, val);\
}\
    void put_be ##name(AVIOContext *s, type val)\
{\
        avio_wb ##name(s, val);\
}

PUT(16, unsigned int)
PUT(24, unsigned int)
PUT(32, unsigned int)
PUT(64, uint64_t)
#undef PUT

int get_byte(AVIOContext *s)
{
   return avio_r8(s);
}
int get_buffer(AVIOContext *s, unsigned char *buf, int size)
{
    return avio_read(s, buf, size);
}
void put_byte(AVIOContext *s, int val)
{
    avio_w8(s, val);
}
void put_buffer(AVIOContext *s, const unsigned char *buf, int size)
{
    avio_write(s, buf, size);
}

int avio_put_str(AVIOContext *s, const char *str)
{
    int len = 1;
    if (str) {
        len += strlen(str);
        avio_write(s, (const unsigned char *) str, len);
    } else
        avio_w8(s, 0);
    return len;
}

void avio_wl64(AVIOContext *s, uint64_t val)
{
    avio_wl32(s, (uint32_t)(val & 0xffffffff));
    avio_wl32(s, (uint32_t)(val >> 32));
}

void avio_wb64(AVIOContext *s, uint64_t val)
{
    avio_wb32(s, (uint32_t)(val >> 32));
    avio_wb32(s, (uint32_t)(val & 0xffffffff));
}

void avio_wl16(AVIOContext *s, unsigned int val)
{
    avio_w8(s, val);
    avio_w8(s, val >> 8);
}

void avio_wb16(AVIOContext *s, unsigned int val)
{
    avio_w8(s, val >> 8);
    avio_w8(s, val);
}

void avio_wl24(AVIOContext *s, unsigned int val)
{
    avio_wl16(s, val & 0xffff);
    avio_w8(s, val >> 16);
}

void avio_wb24(AVIOContext *s, unsigned int val)
{
    avio_wb16(s, val >> 8);
    avio_w8(s, val);
}

unsigned int avio_rl16(AVIOContext *s)
{
    unsigned int val;
    val = avio_r8(s);
    val |= avio_r8(s) << 8;
    return val;
}

unsigned int avio_rl24(AVIOContext *s)
{
    unsigned int val;
    val = avio_rl16(s);
    val |= avio_r8(s) << 16;
    return val;
}

unsigned int avio_rl32(AVIOContext *s)
{
    unsigned int val;
    val = avio_rl16(s);
    val |= avio_rl16(s) << 16;
    return val;
}

uint64_t avio_rl64(AVIOContext *s)
{
    uint64_t val;
    val = (uint64_t)avio_rl32(s);
    val |= (uint64_t)avio_rl32(s) << 32;
    return val;
}

unsigned int avio_rb16(AVIOContext *s)
{
    unsigned int val;
    val = avio_r8(s) << 8;
    val |= avio_r8(s);
    return val;
}

unsigned int avio_rb24(AVIOContext *s)
{
    unsigned int val;
    val = avio_rb16(s) << 8;
    val |= avio_r8(s);
    return val;
}
unsigned int avio_rb32(AVIOContext *s)
{
    unsigned int val;
    val = avio_rb16(s) << 16;
    val |= avio_rb16(s);
    return val;
}

int ff_get_line(AVIOContext *s, char *buf, int maxlen)
{
    int i = 0;
    char c;

    do {
        c = avio_r8(s);
        if (c && i < maxlen-1)
            buf[i++] = c;
    } while (c != '\n' && c);

    buf[i] = 0;
    return i;
}

uint64_t avio_rb64(AVIOContext *s)
{
    uint64_t val;
    val = (uint64_t)avio_rb32(s) << 32;
    val |= (uint64_t)avio_rb32(s);
    return val;
}

uint64_t ffio_read_varlen(AVIOContext *bc){
    uint64_t val = 0;
    int tmp;

    do{
        tmp = avio_r8(bc);
        val= (val<<7) + (tmp&127);
    }while(tmp&128);
    return val;
}

