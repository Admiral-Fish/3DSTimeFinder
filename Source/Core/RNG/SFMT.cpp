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
#include <Core/RNG/SIMD.hpp>

SFMT::SFMT(u32 seed, u32 frames)
{
    initialize(seed);
    advanceFrames(frames);
}

void SFMT::initialize(u32 seed)
{
    u32 inner = seed & 1;
    sfmt[0] = seed;

    for (index = 1; index < 624; index++)
    {
        seed = 0x6C078965 * (seed ^ (seed >> 30)) + index;
        sfmt[index] = seed;
    }

    inner ^= sfmt[3] & 0x13c9e684;
    inner ^= inner >> 16;
    inner ^= inner >> 8;
    inner ^= inner >> 4;
    inner ^= inner >> 2;
    inner ^= inner >> 1;

    sfmt[0] ^= ~inner & 1;
}

void SFMT::advanceFrames(u32 frames)
{
    frames = frames * 2 + index;
    while (frames >= 624)
    {
        shuffle();
        frames -= 624;
    }
    index = frames;
}

u64 SFMT::next()
{
    if (index == 624)
    {
        index = 0;
        shuffle();
    }

    u32 high = sfmt[index++];
    u32 low = sfmt[index++];
    return high | (static_cast<u64>(low) << 32);
}

void SFMT::shuffle()
{
    vuint32x4 c = v32x4_load(&sfmt[616]);
    vuint32x4 d = v32x4_load(&sfmt[620]);
    vuint32x4 mask = v32x4_set(0xdfffffef, 0xddfecb7f, 0xbffaffff, 0xbffffff6);

    auto mm_recursion = [&mask](vuint32x4 &a, const vuint32x4 &b, const vuint32x4 &c, const vuint32x4 &d) {
        vuint32x4 x = v128_shl<1>(a);
        vuint32x4 y = v128_shr<1>(c);

        vuint32x4 b1 = v32x4_and(v32x4_shr<11>(b), mask);
        vuint32x4 d1 = v32x4_shl<18>(d);

        a = v32x4_xor(v32x4_xor(v32x4_xor(v32x4_xor(a, x), b1), y), d1);
    };

    for (int i = 0; i < 136; i += 4)
    {
        vuint32x4 a = v32x4_load(&sfmt[i]);
        vuint32x4 b = v32x4_load(&sfmt[i + 488]);

        mm_recursion(a, b, c, d);
        v32x4_store(&sfmt[i], a);

        c = d;
        d = a;
    }

    for (int i = 136; i < 624; i += 4)
    {
        vuint32x4 a = v32x4_load(&sfmt[i]);
        vuint32x4 b = v32x4_load(&sfmt[i - 136]);

        mm_recursion(a, b, c, d);
        v32x4_store(&sfmt[i], a);

        c = d;
        d = a;
    }
}
