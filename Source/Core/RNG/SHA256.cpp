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

#include "SHA256.hpp"

constexpr u32 K[64] = { 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1,
    0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d,
    0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
    0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3,
    0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb,
    0xbef9a3f7, 0xc67178f2 };

inline u32 rightRotate(u32 val, u8 count)
{
    return (val >> count) | (val << (32 - count));
}

inline u32 changeEndian(u32 num)
{
    return ((num >> 24) & 0xff) | ((num << 8) & 0xff0000) | ((num >> 8) & 0xff00) | ((num << 24) & 0xff000000);
}

u32 SHA256::hash(u32 tick, u32 epochLow, u32 epochHigh)
{
    u32 w[64] = { changeEndian(tick), 0, changeEndian(epochLow), changeEndian(epochHigh), 0x80000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80 };

    // w[16] is always changeEndian(tick) as long as w[1] == 0 is true, precompute
    // This appears to be the case on emulator
    w[16] = changeEndian(tick);

    for (u8 i = 17; i < 64; i++)
    {
        u32 sig0 = rightRotate(w[i - 15], 7) ^ rightRotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
        u32 sig1 = rightRotate(w[i - 2], 17) ^ rightRotate(w[i - 2], 19) ^ (w[i - 2] >> 10);

        w[i] = sig1 + w[i - 7] + sig0 + w[i - 16];
    }

    u32 a = 0x6a09e667;
    u32 b = 0xbb67ae85;
    u32 c = 0x3c6ef372;
    u32 d = 0xa54ff53a;
    u32 e = 0x510e527f;
    u32 f = 0x9b05688c;
    u32 g = 0x1f83d9ab;
    u32 h = 0x5be0cd19;

    for (u8 i = 0; i < 64; i++)
    {
        u32 s1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
        u32 ch = (e & f) ^ ((~e) & g);

        u32 temp1 = h + s1 + ch + K[i] + w[i];

        u32 s0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
        u32 maj = (a & b) ^ (a & c) ^ (b & c);

        u32 temp2 = s0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    return changeEndian(a + 0x6a09e667);
}
