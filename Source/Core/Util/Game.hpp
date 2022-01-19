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

#ifndef GAME_HPP
#define GAME_HPP

enum Game : int
{
    X = 1 << 0,
    Y = 1 << 1,
    XY = X | Y,
    OR = 1 << 2,
    AS = 1 << 3,
    ORAS = OR | AS,
    Sun = 1 << 4,
    Moon = 1 << 5,
    SM = Sun | Moon,
    UltraSun = 1 << 6,
    UltraMoon = 1 << 7,
    USUM = UltraSun | UltraMoon,
};

#endif // GAME_HPP
