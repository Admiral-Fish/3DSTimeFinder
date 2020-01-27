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
#include "WildFilter.hpp"

WildFilter::WildFilter(const QVector<u8> &minIV, const QVector<u8> &maxIV, const QVector<bool> &nature,
    const QVector<bool> &hiddenPower, const QVector<bool> &encounterSlots, u8 ability, bool shiny, u8 gender)
{
    this->minIV = minIV;
    this->maxIV = maxIV;
    this->nature = nature;
    this->hiddenPower = hiddenPower;
    this->encounterSlots = encounterSlots;
    this->ability = ability;
    this->shiny = shiny;
    this->gender = gender;
}

bool WildFilter::compare(const WildResult &frame)
{
    if (shiny && !frame.getShiny())
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

    if (!nature.at(frame.getNature()))
    {
        return false;
    }

    if (!hiddenPower.at(frame.getHiddenPower()))
    {
        return false;
    }

    if (!encounterSlots.at(frame.getEncounterSlot() - 1))
    {
        return false;
    }

    for (u8 i = 0; i < 6; i++)
    {
        u8 iv = frame.getIV(i);
        if (iv < minIV.at(i) || iv > maxIV.at(i))
        {
            return false;
        }
    }

    return true;
}
