/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019 by Admiral_Fish
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

constexpr u32 LOWERMASK = 0x7FFFFFFF;
constexpr u32 UPPERMASK = 0x80000000;
constexpr u16 M = 397;
constexpr u16 N = 624;
constexpr u32 TEMPERINGMASKB = 0x9D2C5680;
constexpr u32 TEMPERINGMASKC = 0xEFC60000;
constexpr u32 mag01[2] = { 0x0, 0x9908B0DF };

MT::MT(u32 seed, u32 frames)
{
    initialize(seed);
    advanceFrames(frames);
}

void MT::initialize(u32 seed)
{
    mt[0] = seed;

    for (index = 1; index < N; index++)
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
    if (index >= N)
    {
        shuffle();
    }

    u32 y = mt[index++];
    y ^= (y >> 11);
    y ^= (y << 7) & TEMPERINGMASKB;
    y ^= (y << 15) & TEMPERINGMASKC;
    y ^= (y >> 18);
    return y;
}

void MT::shuffle()
{
    u16 i = 0;

    for (; i < 227; i++)
    {
        u32 y = (mt[i] & UPPERMASK) | (mt[i + 1] & LOWERMASK);
        mt[i] = mt[i + M] ^ (y >> 1) ^ mag01[y & 1];
    }

    for (; i < 623; i++)
    {
        u32 y = (mt[i] & UPPERMASK) | (mt[i + 1] & LOWERMASK);
        mt[i] = mt[i - 227] ^ (y >> 1) ^ mag01[y & 1];
    }

    u32 y = (mt[623] & UPPERMASK) | (mt[0] & LOWERMASK);
    mt[623] = mt[396] ^ (y >> 1) ^ mag01[y & 1];

    index -= 624;
}
