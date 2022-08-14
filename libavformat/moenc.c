/*
 * MO muxer
 * Copyright (c) 2022 Spotlight Deveaux
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

#include "avio_internal.h"
#include "internal.h"
#include "mo.h"

typedef struct MoMuxContext {
    int header_size;
} MoMuxContext;

static int mo_write_header(AVFormatContext *s)
{
  AVIOContext *pb = s->pb;
  MoMuxContext *mo = s->priv_data;

  // Ensure we have two streams.
  if (s->nb_streams != 2) {
      av_log(s, AV_LOG_ERROR, "Two streams - one video, one audio - are required!\n");
      return AVERROR_STREAM_NOT_FOUND;
  }

  // Get video stream.
  AVStream* video_stream = s->streams[0];
  AVCodecParameters *video_par = video_stream->codecpar;
  if (video_par->codec_type != AVMEDIA_TYPE_VIDEO && video_par->codec_id != AV_CODEC_ID_MOBICLIP) {
    av_log(s, AV_LOG_ERROR, "Only Mobiclip is supported for video!\n");
    return AVERROR_STREAM_NOT_FOUND;
  }

  // Get audio stream.
  AVStream* audio_stream = s->streams[1];
  AVCodecParameters *audio_par = audio_stream->codecpar;
  if (audio_par->codec_type != AVMEDIA_TYPE_AUDIO && audio_par->codec_id != AV_CODEC_ID_PCM_S16LE) {
    av_log(s, AV_LOG_ERROR, "Only PCM is currently supported for audio!\n");
    return AVERROR_PATCHWELCOME;
  }

  // ============
  // Time to begin constructing the header.
  // ============

  // Container format identifier.
  avio_wl32(pb, MKTAG('M', 'O', 'C', '5'));
  // Length of header. This will be overwritten in the trailer.
  avio_wl32(pb, 0);

  // We've written 8 bytes so far.
  mo->header_size = 8;

  avio_wl16(pb, FORMAT_LENGTH);
  avio_wl16(pb, 3); // 3 uint32_ts within this format segment.
  // Compose frame rate
  // TODO: this is so horribly wrong lol
  avio_wl32(pb, video_stream->avg_frame_rate.num * 256); // FPS * 256.0
  avio_wl32(pb, video_stream->nb_frames);
  avio_wl32(pb, 0); // TODO: what is this? Its value appears to be '<< 1' within official software.
  mo->header_size += 16;

  avio_wl16(pb, FORMAT_VIDEO);
  avio_wl16(pb, 2);
  avio_wl32(pb, video_par->width);
  avio_wl32(pb, video_par->height);
  mo->header_size += 12;

  // TODO: possibly allow specifying signature via a flag, if desired
  avio_wl16(pb, FORMAT_RSA);
  avio_wl16(pb, 40);
  // TODO!!!!! REMOVE
  const unsigned char rsa_key[] = {0x2B, 0x8F, 0x50, 0x80, 0xE3, 0x23, 0x38, 0xCE, 0x86, 0x70, 0xDD, 0xA6, 0xE0, 0x19, 0xE3, 0xBB, 0xC5, 0xDB, 0xE6, 0x12, 0x55, 0x06, 0x7A, 0x46, 0x1F, 0xEE, 0xD5, 0xBE, 0x40, 0x41, 0xBB, 0x60, 0x71, 0x50, 0xD6, 0x1D, 0x1D, 0x8B, 0xD5, 0xDB, 0xF2, 0x2C, 0x19, 0x73, 0xDA, 0x60, 0xAC, 0xF0, 0xF4, 0xD2, 0x68, 0xBF, 0x47, 0x21, 0x7B, 0x92, 0x66, 0x1E, 0xDD, 0x31, 0x59, 0x6D, 0xFA, 0x9F, 0xF7, 0x6A, 0x6B, 0xC5, 0x5E, 0x2B, 0xF9, 0x78, 0xB3, 0x7B, 0x3A, 0xA6, 0x65, 0x87, 0x16, 0x05, 0x38, 0xC6, 0x2B, 0x29, 0x27, 0xA9, 0x42, 0xC0, 0x0E, 0x1B, 0x96, 0xF3, 0xD3, 0xC5, 0x21, 0xF4, 0xBD, 0x0E, 0x7B, 0xB3, 0xCF, 0x00, 0x90, 0xEE, 0xA4, 0xA4, 0x85, 0xDC, 0xEB, 0x0F, 0xF2, 0x40, 0xC8, 0xD1, 0x46, 0x85, 0x55, 0x0F, 0xC5, 0x71, 0xE6, 0xF7, 0xE3, 0xD5, 0x45, 0x46, 0xA4, 0xC6, 0xE3, 0xCC, 0xBD, 0xD4, 0x85, 0x7F, 0xBA, 0x1B, 0xA0, 0x9C, 0x0D, 0xE3, 0x19, 0x70, 0x63, 0x26, 0x45, 0xA7, 0x12, 0xAC, 0xD3, 0x1E, 0x95, 0x0A, 0xD4, 0x46, 0x72, 0x7B, 0xAD, 0xF6, 0xEB, 0xE3};
  avio_write(pb, rsa_key, 160);
  mo->header_size += 164;

  // TODO: do we want to handle PÆ/FORMAT_UNKNOWN_AUDIO?
  avio_wl16(pb, FORMAT_PCM);
  avio_wl16(pb, 2);
  avio_wl32(pb, audio_par->sample_rate);
  avio_wl32(pb, audio_par->ch_layout.nb_channels);
  mo->header_size += 12;

  // TODO: add KI - while it's technically not needed, things will absolutely crash
  // TODO: what's cc? It doesn't seem to be necessary.

  // Lastly, the end of our header.
  avio_wl16(pb, FORMAT_HEADER_DONE);
  avio_wl16(pb, 0);
  mo->header_size += 4;

  return 0;
}

static int mo_write_packet(AVFormatContext *s, AVPacket *pkt)
{
  return 0;
}

const AVOutputFormat ff_mo_muxer = {
    .name         = "mobiclip_mo",
    .long_name    = NULL_IF_CONFIG_SMALL("MobiClip MO"),
    .extensions   = "mo",
    .priv_data_size = sizeof(MoMuxContext),
    .audio_codec  = AV_CODEC_ID_PCM_S16LE,
    .video_codec  = AV_CODEC_ID_MOBICLIP,
    .write_header = mo_write_header,
    .write_packet = mo_write_packet,
};
