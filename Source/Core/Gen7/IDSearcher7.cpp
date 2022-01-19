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

#include "IDSearcher7.hpp"
#include <Core/RNG/SFMT.hpp>
#include <Core/Util/Utility.hpp>
#include <Core/Parents/IDResult.hpp>
#include <future>

IDSearcher7::IDSearcher7(const DateTime &startTime, const DateTime &endTime, u32 startFrame, u32 endFrame, const Profile7 &profile,
                         const IDFilter &filter) :
    startTime(startTime),
    endTime(endTime),
    startFrame(startFrame),
    endFrame(endFrame),
    filter(filter),
    profile(profile),
    progress(0),
    searching(false)
{
}

void IDSearcher7::startSearch(int threads)
{
    searching = true;

    u64 epochStart = Utility::getCitraTime(startTime, profile.getOffset());
    u64 epochEnd = Utility::getCitraTime(endTime, profile.getOffset());

    u64 epochSplit = (epochEnd - epochStart) / threads;
    epochSplit -= (epochSplit % 1000); // Floor to nearest thousand

    if (epochSplit < 1000)
    {
        threads = 1;
    }

    std::vector<std::future<void>> threadContainer;
    for (int i = 0; i < threads; i++)
    {
        if (i == threads - 1)
        {
            threadContainer.emplace_back(std::async(std::launch::async, [=] { search(epochStart, epochEnd); }));
        }
        else
        {
            threadContainer.emplace_back(std::async(std::launch::async, [=] { search(epochStart, epochStart + epochSplit); }));
        }
        epochStart += epochSplit;
    }

    for (int i = 0; i < threads; i++)
    {
        threadContainer[i].wait();
    }
}

void IDSearcher7::cancelSearch()
{
    searching = false;
}

int IDSearcher7::getProgress() const
{
    return progress;
}

int IDSearcher7::getMaxProgress() const
{
    auto val = static_cast<int>(
        (Utility::getCitraTime(endTime, profile.getOffset()) - Utility::getCitraTime(startTime, profile.getOffset())) / 1000);
    return val + 1;
}

std::vector<IDResult> IDSearcher7::getResults()
{
    std::lock_guard<std::mutex> lock(mutex);
    auto data = std::move(results);
    return data;
}

void IDSearcher7::search(u64 epochStart, u64 epochEnd)
{
    u32 tick = profile.getTick();
    u32 offset = profile.getOffset();

    DateTime target = DateTime(Utility::getNormalTime(epochStart, offset));
    for (u64 epoch = epochStart; epoch <= epochEnd && searching; epoch += 1000)
    {
        u32 initialSeed = Utility::calcInitialSeed(tick, epoch);

        SFMT sfmt(initialSeed, startFrame);

        for (u32 frame = startFrame; frame <= endFrame; frame++)
        {
            IDResult id(initialSeed, frame, sfmt.next() & 0xffffffff);
            if (filter.compare(id))
            {
                id.setTarget(target);

                std::lock_guard<std::mutex> lock(mutex);
                results.emplace_back(id);
            }
        }
        progress++;
    }
}
