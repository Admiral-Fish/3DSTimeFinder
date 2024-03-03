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

#ifndef PROFILESEARCHER7_HPP
#define PROFILESEARCHER7_HPP

#include <Core/Util/Global.hpp>
#include <mutex>
#include <vector>

class DateTime;

class ProfileSearcher7
{
public:
    ProfileSearcher7(const DateTime &startDate, u32 initialSeed, u32 baseTick, u32 baseOffset, u32 tickRange, u32 offsetRange);
    void startSearch();
    void cancelSearch();
    int getProgress() const;
    int getMaxProgress() const;
    std::vector<std::pair<u32, u32>> getResults();

private:
    u64 epochBase;
    u32 initialSeed;
    u32 baseTick, baseOffset;
    u32 tickRange, offsetRange;

    std::vector<std::pair<u32, u32>> results;
    std::mutex mutex;
    int progress;
    bool searching;

    void search();
};

#endif // PROFILESEARCHER7_HPP
