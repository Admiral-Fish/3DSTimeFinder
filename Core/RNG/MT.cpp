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

#include "MT.hpp"
#include <cstring>

MT::MT(u32 seed, u32 frames)
{
    initialize(seed);
    advanceFrames(frames);
}

void MT::initialize(u32 seed)
{
    mt[0] = seed;

    for (index = 1; index < 624; index++)
    {
        mt[index] = (0x6C078965 * (mt[index - 1] ^ (mt[index - 1] >> 30))) + index;
    }
}

void MT::advanceFrames(u32 frames)
{
    index += frames;
    while (index >= 624)
    {
        shuffle();
    }
}

u32 MT::nextUInt()
{
    if (index >= 624)
    {
        shuffle();
    }

    u32 y = mt[index++];
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9D2C5680;
    y ^= (y << 15) & 0xEFC60000;
    y ^= (y >> 18);
    return y;
}

void MT::shuffle()
{
    for (u16 i = 0; i < 624; i++)
    {
        u32 y = (mt[i] & 0x80000000) | (mt[(i + 1) % 624] & 0x7FFFFFFF);
        u32 next = y >> 1;

        if (y & 1)
        {
            next ^= 0x9908B0DF;
        }

        mt[i] = next ^ mt[(i + 397) % 624];
    }

    index -= 624;
}
