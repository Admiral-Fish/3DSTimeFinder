/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2024 by Admiral_Fish
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

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <Core/Util/Global.hpp>

enum class Controls : u16
{
    None = 0,
    IVs = 1 << 0,
    Ability = 1 << 1,
    Gender = 1 << 2,
    EncounterSlots = 1 << 3,
    HiddenPowers = 1 << 4,
    Natures = 1 << 5,
    Shiny = 1 << 6,
};

constexpr Controls operator|(Controls left, Controls right)
{
    return static_cast<Controls>(static_cast<u16>(left) | static_cast<u16>(right));
};

constexpr Controls operator&(Controls left, Controls right)
{
    return static_cast<Controls>(static_cast<u16>(left) & static_cast<u16>(right));
};

#endif // CONTROLS_HPP
