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

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <Core/Util/Global.hpp>
#include <string>
#include <vector>

class DateTime;

namespace Utility
{
    u64 getCitraTime(const DateTime &dt, u64 offset = 0);
    u64 getNormalTime(u64 time, u64 offset = 0);
    u32 calcInitialSeed(u32 tick, u64 epoch);
    const std::string &getNature(u8 nature);
    const std::string &getHiddenPower(u8 hiddenPower);
    const std::vector<std::string> &getNatures();
    const std::vector<std::string> &getHiddenPowers();
    const std::vector<std::string> &getGenderRatios();
}

#endif // UTILITY_HPP
