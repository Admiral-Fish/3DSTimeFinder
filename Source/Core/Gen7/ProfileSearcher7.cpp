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

#include "ProfileSearcher7.hpp"
#include <Core/Util/Utility.hpp>
#include <future>

ProfileSearcher7::ProfileSearcher7(const DateTime &startDate, u32 initialSeed, u32 baseTick, u32 baseOffset, u32 tickRange,
                                   u32 offsetRange) :
    epochBase(startDate.toMSecsSinceEpoch()),
    initialSeed(initialSeed),
    baseTick(baseTick),
    baseOffset(baseOffset),
    tickRange(tickRange),
    offsetRange(offsetRange),
    progress(0),
    searching(false)
{
}

void ProfileSearcher7::startSearch()
{
    searching = true;
    search();
}

void ProfileSearcher7::cancelSearch()
{
    searching = false;
}

int ProfileSearcher7::getProgress() const
{
    return progress;
}

int ProfileSearcher7::getMaxProgress() const
{
    return static_cast<int>(tickRange + 1);
}

std::vector<std::pair<u32, u32>> ProfileSearcher7::getResults()
{
    std::lock_guard<std::mutex> lock(mutex);
    auto data = std::move(results);
    return data;
}

void ProfileSearcher7::search()
{
    for (u32 tick = 0; tick <= tickRange; tick++)
    {
        for (u32 offset = 0; offset <= offsetRange; offset++)
        {
            if (!searching)
            {
                return;
            }

            // Plus offset
            u64 epochPlus = epochBase + baseOffset + offset;
            u32 seedPlus = Utility::calcInitialSeed(baseTick + tick, epochPlus);
            if (seedPlus == initialSeed)
            {
                std::lock_guard<std::mutex> lock(mutex);
                results.emplace_back(baseTick + tick, baseOffset + offset);
            }

            // Minus offset
            u64 epochMinus = epochBase + baseOffset - offset;
            u32 seedMinus = Utility::calcInitialSeed(baseTick - tick, epochMinus);
            if (seedMinus == initialSeed)
            {
                std::lock_guard<std::mutex> lock(mutex);
                results.emplace_back(baseTick - tick, baseOffset - offset);
            }
        }

        progress++;
    }
}
