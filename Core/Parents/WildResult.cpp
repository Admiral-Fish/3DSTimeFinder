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

#include "WildResult.hpp"

WildResult::WildResult(u32 seed, u16 tid, u16 sid)
{
    this->seed = seed;
    tsv = (tid ^ sid) >> 4;
    ivs.fill(-1, 6);
}

bool WildResult::getSynch() const
{
    return synch;
}

void WildResult::setSynch(const bool &synch)
{
    this->synch = synch;
}

u8 WildResult::getEncounterSlot() const
{
    return encounterSlot;
}

void WildResult::setEncounterSlot(const u8 &encounterSlot)
{
    this->encounterSlot = encounterSlot;
}
