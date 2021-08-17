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

#ifndef IDRESULT_HPP
#define IDRESULT_HPP

#include <Core/Util/DateTime.hpp>
#include <Core/Util/Global.hpp>
#include <string>

class IDResult
{
public:
    IDResult() = default;
    IDResult(u32 seed, u32 frame, u32 rand);
    std::string getDateTime() const;
    u32 getSeed() const;
    u32 getFrame() const;
    u16 getTID() const;
    u16 getSID() const;
    u16 getTSV() const;
    u32 getDisplayTID() const;
    void setTarget(const DateTime &target);

private:
    DateTime target;
    u32 seed, frame, displayTID;
    u16 tid, sid, tsv;
};

#endif // IDRESULT_HPP
