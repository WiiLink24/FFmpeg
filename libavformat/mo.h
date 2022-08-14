/*
 * Shared tags for MO muxer/demuxer
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

#define MO_TAG MKTAG('M', 'O', 'C', '5')

#define FORMAT_MARKER(a,b) ((a) | ((b) << 8))

// TODO: can this become a better formatted enum?
// What C specification are we using?

// Metadata types
#define FORMAT_LENGTH             FORMAT_MARKER('T', 'L')
#define FORMAT_VIDEO              FORMAT_MARKER('V', '2')
#define FORMAT_RSA                FORMAT_MARKER('p', 'c')

// Unknown
#define FORMAT_UNKNOWN_AUDIO      FORMAT_MARKER('P', 0xc6)

// Audio types
#define FORMAT_FASTAUDIO          FORMAT_MARKER('A', '2')
#define FORMAT_FASTAUDIO_STEREO   FORMAT_MARKER('A', '3')
#define FORMAT_PCM                FORMAT_MARKER('A', 'P')
#define FORMAT_ADPCM              FORMAT_MARKER('A', '8')
#define FORMAT_ADPCM_STEREO       FORMAT_MARKER('A', '9')
#define FORMAT_MULTITRACK         FORMAT_MARKER('A', 'M')
#define FORMAT_VORBIS             FORMAT_MARKER('A', 'V')

// Video metadata
#define FORMAT_KEYINDEX           FORMAT_MARKER('K', 'I')
#define FORMAT_POSSIBLY_CAPTIONS  FORMAT_MARKER('c', 'c')
#define FORMAT_HEADER_DONE        FORMAT_MARKER('H', 'E')