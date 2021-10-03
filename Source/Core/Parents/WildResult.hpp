/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2021 by Admiral_Fish
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

#ifndef WILDRESULT_HPP
#define WILDRESULT_HPP

#include <Core/Parents/Result.hpp>

class WildResult : public Result
{
public:
    WildResult() = default;

    WildResult(u32 seed, u16 tid, u16 sid) : Result(seed, tid, sid)
    {
    }

    bool getSynch() const
    {
        return synch;
    }

    void setSynch(bool synch)
    {
        this->synch = synch;
    };

    u8 getEncounterSlot() const
    {
        return encounterSlot;
    }

    void setEncounterSlot(u8 encounterSlot)
    {
        this->encounterSlot = encounterSlot;
    }

private:
    bool synch;
    u8 encounterSlot;
};

#endif // WILDRESULT_HPP
