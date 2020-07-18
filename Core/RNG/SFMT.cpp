/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2020 by Admiral_Fish
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "SFMT.hpp"

SFMT::SFMT(u32 seed, u32 frames)
{
    initialize(seed);
    advanceFrames(frames);
}

void SFMT::initialize(u32 seed)
{
    sfmt[0] = seed;

    for (index = 1; index < 624; index++)
    {
        sfmt[index] = (0x6C078965 * (sfmt[index - 1] ^ (sfmt[index - 1] >> 30))) + index;
    }

    u32 inner = (sfmt[0] & 1) ^ (sfmt[3] & 0x13c9e684);
    inner ^= inner >> 16;
    inner ^= inner >> 8;
    inner ^= inner >> 4;
    inner ^= inner >> 2;
    inner ^= inner >> 1;

    sfmt[0] ^= ~inner & 1;
}

void SFMT::advanceFrames(u32 frames)
{
    index += frames * 2;
    while (index >= 624)
    {
        shuffle();
    }
}

u32 SFMT::nextUInt()
{
    if (index >= 624)
    {
        shuffle();
    }

    return sfmt[index++];
}

u64 SFMT::nextULong()
{
    if (index >= 624)
    {
        shuffle();
    }

    u32 high = sfmt[index++];
    u32 low = sfmt[index++];
    return high | (static_cast<u64>(low) << 32);
}

void SFMT::shuffle()
{
    u16 a = 0;
    u16 b = 488;
    u16 c = 616;
    u16 d = 620;

    do
    {
        sfmt[a + 3] = sfmt[a + 3] ^ (sfmt[a + 3] << 8) ^ (sfmt[a + 2] >> 24) ^ (sfmt[c + 3] >> 8) ^ ((sfmt[b + 3] >> 11) & 0xbffffff6)
            ^ (sfmt[d + 3] << 18);
        sfmt[a + 2] = sfmt[a + 2] ^ (sfmt[a + 2] << 8) ^ (sfmt[a + 1] >> 24) ^ (sfmt[c + 3] << 24) ^ (sfmt[c + 2] >> 8)
            ^ ((sfmt[b + 2] >> 11) & 0xbffaffff) ^ (sfmt[d + 2] << 18);
        sfmt[a + 1] = sfmt[a + 1] ^ (sfmt[a + 1] << 8) ^ (sfmt[a] >> 24) ^ (sfmt[c + 2] << 24) ^ (sfmt[c + 1] >> 8)
            ^ ((sfmt[b + 1] >> 11) & 0xddfecb7f) ^ (sfmt[d + 1] << 18);
        sfmt[a] = sfmt[a] ^ (sfmt[a] << 8) ^ (sfmt[c + 1] << 24) ^ (sfmt[c] >> 8) ^ ((sfmt[b] >> 11) & 0xdfffffef) ^ (sfmt[d] << 18);
        c = d;
        d = a;
        a += 4;
        b += 4;
        if (b >= 624)
        {
            b = 0;
        }
    } while (a < 624);

    index -= 624;
}
