/*
 * MLP parser
 * Copyright (c) 2007 Ian Caulfield
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

/**
 * @file
 * MLP parser
 */

#include <stdint.h>

#include "misc.h"
#include "crc.h"
#include "get_bits.h"
#include "mlp_parser.h"
#include "mlp.h"
#include "audioconvert.h"

static const uint8_t mlp_quants[16] = {
    16, 20, 24, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
};

static const uint8_t mlp_channels[32] = {
    1, 2, 3, 4, 3, 4, 5, 3, 4, 5, 4, 5, 6, 4, 5, 4,
    5, 6, 5, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const uint64_t ff_mlp_layout[32] = {
    AV_CH_LAYOUT_MONO,
    AV_CH_LAYOUT_STEREO,
    AV_CH_LAYOUT_2_1,
    AV_CH_LAYOUT_QUAD,
    AV_CH_LAYOUT_STEREO|AV_CH_LOW_FREQUENCY,
    AV_CH_LAYOUT_2_1|AV_CH_LOW_FREQUENCY,
    AV_CH_LAYOUT_QUAD|AV_CH_LOW_FREQUENCY,
    AV_CH_LAYOUT_SURROUND,
    AV_CH_LAYOUT_4POINT0,
    AV_CH_LAYOUT_5POINT0_BACK,
    AV_CH_LAYOUT_SURROUND|AV_CH_LOW_FREQUENCY,
    AV_CH_LAYOUT_4POINT0|AV_CH_LOW_FREQUENCY,
    AV_CH_LAYOUT_5POINT1_BACK,
    AV_CH_LAYOUT_4POINT0,
    AV_CH_LAYOUT_5POINT0_BACK,
    AV_CH_LAYOUT_SURROUND|AV_CH_LOW_FREQUENCY,
    AV_CH_LAYOUT_4POINT0|AV_CH_LOW_FREQUENCY,
    AV_CH_LAYOUT_5POINT1_BACK,
    AV_CH_LAYOUT_QUAD|AV_CH_LOW_FREQUENCY,
    AV_CH_LAYOUT_5POINT0_BACK,
    AV_CH_LAYOUT_5POINT1_BACK,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const uint8_t thd_chancount[13] = {
//  LR    C   LFE  LRs LRvh  LRc LRrs  Cs   Ts  LRsd  LRw  Cvh  LFE2
     2,   1,   1,   2,   2,   2,   2,   1,   1,   2,   2,   1,   1
};

static const uint64_t thd_layout[13] = {
    AV_CH_FRONT_LEFT|AV_CH_FRONT_RIGHT,                     // LR
    AV_CH_FRONT_CENTER,                                     // C
    AV_CH_LOW_FREQUENCY,                                    // LFE
    AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT,                       // LRs
    AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT,             // LRvh
    AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER, // LRc
    AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT,                       // LRrs
    AV_CH_BACK_CENTER,                                      // Cs
    AV_CH_TOP_CENTER,                                       // Ts
    AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT,                       // LRsd - TODO: Surround Direct
    AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER, // LRw  - TODO: Wide
    AV_CH_TOP_FRONT_CENTER,                                 // Cvh
    AV_CH_LOW_FREQUENCY                                     // LFE2
};

static int mlp_samplerate(int in)
{
    if (in == 0xF)
        return 0;

    return (in & 8 ? 44100 : 48000) << (in & 7) ;
}

static int truehd_channels(int chanmap)
{
    int channels = 0, i;

    for (i = 0; i < 13; i++)
        channels += thd_chancount[i] * ((chanmap >> i) & 1);

    return channels;
}

uint64_t ff_truehd_layout(int chanmap)
{
    int layout = 0, i;

    for (i = 0; i < 13; i++)
        layout |= thd_layout[i] * ((chanmap >> i) & 1);

    return layout;
}

/** Read a major sync info header - contains high level information about
 *  the stream - sample rate, channel arrangement etc. Most of this
 *  information is not actually necessary for decoding, only for playback.
 *  gb must be a freshly initialized GetBitContext with no bits read.
 */

int ff_mlp_read_major_sync(void *log, MLPHeaderInfo *mh, GetBitContext *gb)
{
    int ratebits;
    uint16_t checksum;

    //assert(get_bits_count(gb) == 0);

    if (gb->size_in_bits < 28 << 3) {
        return -1;
    }

    checksum = ff_mlp_checksum16(gb->buffer, 26);
    if (checksum != AV_RL16(gb->buffer+26)) {
        return AVERROR_INVALIDDATA;
    }

    if (get_bits_long(gb, 24) != 0xf8726f) /* Sync words */
        return AVERROR_INVALIDDATA;

    mh->stream_type = get_bits(gb, 8);

    if (mh->stream_type == 0xbb) {
        mh->group1_bits = mlp_quants[get_bits(gb, 4)];
        mh->group2_bits = mlp_quants[get_bits(gb, 4)];

        ratebits = get_bits(gb, 4);
        mh->group1_samplerate = mlp_samplerate(ratebits);
        mh->group2_samplerate = mlp_samplerate(get_bits(gb, 4));

        skip_bits(gb, 11);

        mh->channels_mlp = get_bits(gb, 5);
    } else if (mh->stream_type == 0xba) {
        mh->group1_bits = 24; // TODO: Is this information actually conveyed anywhere?
        mh->group2_bits = 0;

        ratebits = get_bits(gb, 4);
        mh->group1_samplerate = mlp_samplerate(ratebits);
        mh->group2_samplerate = 0;

        skip_bits(gb, 8);

        mh->channels_thd_stream1 = get_bits(gb, 5);

        skip_bits(gb, 2);

        mh->channels_thd_stream2 = get_bits(gb, 13);
    } else
        return AVERROR_INVALIDDATA;

    mh->access_unit_size = 40 << (ratebits & 7);
    mh->access_unit_size_pow2 = 64 << (ratebits & 7);

    skip_bits_long(gb, 48);

    mh->is_vbr = get_bits1(gb);

    mh->peak_bitrate = (get_bits(gb, 15) * mh->group1_samplerate + 8) >> 4;

    mh->num_substreams = get_bits(gb, 4);

    skip_bits_long(gb, 4 + 11 * 8);

    return 0;
}

int ffcdecl ffcodec_ff_mlp_get_audio_info(const MLPHeaderInfo *mh,AVAudioInfo* avctx)
{
    if (mh->stream_type == 0xbb) {
        /* MLP stream */
        avctx->channels = mlp_channels[mh->channels_mlp];
        avctx->channel_layout = ff_mlp_layout[mh->channels_mlp];
    } else { /* mh.stream_type == 0xba */
        /* TrueHD stream */
        if (mh->channels_thd_stream2) {
            avctx->channels = truehd_channels(mh->channels_thd_stream2);
            avctx->channel_layout = ff_truehd_layout(mh->channels_thd_stream2);
        } else {
            avctx->channels = truehd_channels(mh->channels_thd_stream1);
            avctx->channel_layout = ff_truehd_layout(mh->channels_thd_stream1);
        }
    }
    return 0;
}

