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

#ifndef WILDSEARCHER7_HPP
#define WILDSEARCHER7_HPP

#include <Core/Gen7/Profile7.hpp>
#include <Core/Parents/WildFilter.hpp>
#include <Core/Util/DateTime.hpp>
#include <atomic>
#include <mutex>

class WildResult;
enum WildType;

class WildSearcher7
{
public:
    WildSearcher7(const DateTime &startTime, const DateTime &endTime, u32 startFrame, u32 endFrame, bool useSynch, u8 synchNature,
                  WildType type, u8 gender, const Profile7 &profile, const WildFilter &filter);
    void startSearch(int threads);
    void cancelSearch();
    int getProgress() const;
    int getMaxProgress() const;
    std::vector<WildResult> getResults();

private:
    Profile7 profile;
    WildFilter filter;
    DateTime startTime, endTime;
    u32 startFrame, endFrame;
    u8 synchNature, pidCount, gender;
    bool useSynch;
    WildType type;

    std::vector<WildResult> results;
    std::mutex mutex;
    std::atomic_int progress;
    bool searching;

    void search(u64 epochStart, u64 epochEnd);
    u8 getSlot(u8 value);
};

#endif // WILDSEARCHER7_HPP
