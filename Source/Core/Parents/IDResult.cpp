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

#include "IDResult.hpp"

IDResult::IDResult(u32 seed, u32 frame, u32 rand) :
    seed(seed), frame(frame), displayTID(rand % 1000000), tid(rand & 0xffff), sid(rand >> 16), tsv((tid ^ sid) >> 4)
{
}

std::string IDResult::getDateTime() const
{
    return target.toString();
}

u32 IDResult::getSeed() const
{
    return seed;
}

u32 IDResult::getFrame() const
{
    return frame;
}

u16 IDResult::getTID() const
{
    return tid;
}

u16 IDResult::getSID() const
{
    return sid;
}

u16 IDResult::getTSV() const
{
    return tsv;
}

u32 IDResult::getDisplayTID() const
{
    return displayTID;
}

void IDResult::setTarget(const DateTime &target)
{
    this->target = target;
}
