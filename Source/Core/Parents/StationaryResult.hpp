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

#ifndef STATIONARYRESULT_HPP
#define STATIONARYRESULT_HPP

#include <Core/Parents/Result.hpp>

class StationaryResult : public Result
{
public:
    StationaryResult() = default;

    StationaryResult(u32 seed, u16 tid, u16 sid) : Result(seed, tid, sid)
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

private:
    bool synch;
};

#endif // STATIONARYRESULT_HPP
