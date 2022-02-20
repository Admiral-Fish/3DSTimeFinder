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

#include "StationaryFilter.hpp"
#include <Core/Parents/StationaryResult.hpp>

StationaryFilter::StationaryFilter(const std::array<u8, 6> &minIV, const std::array<u8, 6> &maxIV, const std::vector<bool> &nature,
                                   const std::vector<bool> &hiddenPower, u8 ability, u8 shiny, u8 gender) :
    ResultFilter(minIV, maxIV, nature, hiddenPower, ability, shiny, gender)
{
}

bool StationaryFilter::compare(const StationaryResult &frame)
{
    if (shiny != 255 && !(shiny & frame.getShiny()))
    {
        return false;
    }

    if (ability != 255 && ability != frame.getAbility())
    {
        return false;
    }

    if (gender != 255 && gender != frame.getGender())
    {
        return false;
    }

    if (!nature[frame.getNature()])
    {
        return false;
    }

    if (!hiddenPower[frame.getHiddenPower()])
    {
        return false;
    }

    for (u8 i = 0; i < 6; i++)
    {
        u8 iv = frame.getIV(i);
        if (iv < minIV[i] || iv > maxIV[i])
        {
            return false;
        }
    }

    return true;
}
