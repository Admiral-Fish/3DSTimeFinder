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

#ifndef RESULT_HPP
#define RESULT_HPP

#include <Core/Util/DateTime.hpp>
#include <Core/Util/Global.hpp>
#include <array>
#include <string>

class Result
{
public:
    Result() = default;
    Result(u32 seed, u16 tid, u16 sid);
    std::string getDateTime() const;
    void setTarget(const DateTime &target);
    u32 getSeed() const;
    u32 getPID() const;
    void setPID(const u32 pid);
    u32 getEC() const;
    void setEC(const u32 ec);
    u32 getFrame() const;
    void setFrame(const u32 frame);
    u8 getHiddenPower() const;
    void calcHiddenPower();
    std::string getAbilityString() const;
    u8 getAbility() const;
    void setAbility(const u8 ability);
    u8 getNature() const;
    void setNature(const u8 nature);
    std::string getGenderString() const;
    u8 getGender() const;
    void setGender(const u8 gender);
    u8 getIV(u8 index) const;
    void setIV(const u8 index, const u8 iv);
    void setIVs(const std::array<u8, 6> &ivs);
    u8 getShiny() const;

protected:
    DateTime target;
    u32 seed, pid, ec, frame;
    u16 tsv;
    u8 hiddenPower, ability, nature, gender, shiny;
    std::array<u8, 6> ivs;
};

#endif // RESULT_HPP
