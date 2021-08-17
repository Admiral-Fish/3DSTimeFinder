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

#ifndef FILTER_HPP
#define FILTER_HPP

#include <Core/Util/Global.hpp>
#include <array>
#include <vector>

class Filter
{
public:
    Filter(const std::array<u8, 6> &minIV, const std::array<u8, 6> &maxIV, const std::vector<bool> &nature,
           const std::vector<bool> &hiddenPower, u8 ability, bool shiny, u8 gender) :
        minIV(minIV), maxIV(maxIV), nature(nature), hiddenPower(hiddenPower), ability(ability), gender(gender), shiny(shiny)
    {
    }

protected:
    std::array<u8, 6> minIV, maxIV;
    std::vector<bool> nature, hiddenPower;
    u8 ability, gender;
    bool shiny;
};

#endif // FILTER_HPP
