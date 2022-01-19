/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2022 by Admiral_Fish
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

#ifndef MT_HPP
#define MT_HPP

#include <Core/Util/Global.hpp>

class MT
{
public:
    explicit MT(u32 seed, u32 frames = 0);
    void advanceFrames(u32 frames);
    u32 next();

private:
    alignas(16) u32 mt[624];
    u16 index;

    void initialize(u32 seed);
    void shuffle();
};

#endif // MT_HPP
