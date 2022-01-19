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

#include "StationarySearcher7.hpp"
#include <Core/Parents/StationaryResult.hpp>
#include <Core/RNG/RNGList.hpp>
#include <Core/RNG/SFMT.hpp>
#include <Core/Util/Utility.hpp>
#include <future>

StationarySearcher7::StationarySearcher7(const DateTime &startTime, const DateTime &endTime, u32 startFrame, u32 endFrame, bool ivCount,
                                         u8 ability, u8 synchNature, u8 gender, bool alwaysSynch, bool shinyLocked, const Profile7 &profile,
                                         const StationaryFilter &filter) :
    profile(profile),
    filter(filter),
    startTime(startTime),
    endTime(endTime),
    startFrame(startFrame),
    endFrame(endFrame),
    ivCount(ivCount ? 3 : 0),
    ability(ability),
    synchNature(synchNature),
    pidCount(profile.getShinyCharm() ? 3 : 1),
    gender(gender),
    alwaysSynch(alwaysSynch),
    shinyLocked(shinyLocked),
    progress(0),
    searching(false)
{
}

void StationarySearcher7::startSearch(int threads)
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

void StationarySearcher7::cancelSearch()
{
    searching = false;
}

int StationarySearcher7::getProgress() const
{
    return progress;
}

int StationarySearcher7::getMaxProgress() const
{
    auto val = static_cast<int>(
        (Utility::getCitraTime(endTime, profile.getOffset()) - Utility::getCitraTime(startTime, profile.getOffset())) / 1000);
    return val + 1;
}

std::vector<StationaryResult> StationarySearcher7::getResults()
{
    std::lock_guard<std::mutex> lock(mutex);
    auto data = std::move(results);
    return data;
}

void StationarySearcher7::search(u64 epochStart, u64 epochEnd)
{
    u32 tick = profile.getTick();
    u32 offset = profile.getOffset();
    u16 tid = profile.getTID();
    u16 sid = profile.getSID();

    DateTime target(Utility::getNormalTime(epochStart, offset));
    for (u64 epoch = epochStart; epoch <= epochEnd && searching; epoch += 1000, target.addSeconds(1))
    {
        u32 initialSeed = Utility::calcInitialSeed(tick, epoch);

        SFMT sfmt(initialSeed, startFrame);
        RNGList<u64, SFMT, 64> rngList(sfmt);

        for (u32 frame = startFrame; frame <= endFrame; frame++, rngList.advanceState())
        {
            StationaryResult result(initialSeed, tid, sid);

            // TODO
            /*
            //Synchronize
            if (alwaysSynch)
                result.setSynch(true);
            else
            {
                rt.Synchronize = blink_process();
                Advance(60);
            }*/

            result.setEC(rngList.getValue() & 0xffffffff);

            for (u8 i = 0; i < pidCount; i++)
            {
                result.setPID(rngList.getValue() & 0xffffffff);
                if (result.getShiny())
                {
                    if (shinyLocked)
                    {
                        result.setPID(result.getPID() ^ 0x10000000);
                    }
                    break;
                }
                // Handle eventually ???
                /*else if (IsForcedShiny)
                {
                    rt.Shiny = true;
                    rt.PID = (uint)((((TSV << 4) ^ (rt.PID & 0xFFFF)) << 16) + (rt.PID & 0xFFFF)); // Not accurate
                }*/
            }

            for (u8 i = 0; i < ivCount;)
            {
                u8 tmp = rngList.getValue() % 6;
                if (result.getIV(tmp) == 255)
                {
                    result.setIV(tmp, 31);
                    i++;
                }
            }

            for (u8 i = 0; i < 6; i++)
            {
                if (result.getIV(i) == 255)
                {
                    result.setIV(i, rngList.getValue() & 0x1f);
                }
            }
            result.calcHiddenPower();

            result.setAbility(ability != 255 ? ability : rngList.getValue() & 1);

            result.setNature(alwaysSynch ? synchNature : rngList.getValue() % 25);

            result.setGender((gender > 0 && gender < 254) ? (rngList.getValue() % 252 < gender) : gender);

            if (filter.compare(result))
            {
                result.setTarget(target);
                result.setFrame(frame);

                std::lock_guard<std::mutex> lock(mutex);
                results.emplace_back(result);
            }
        }
        progress++;
    }
}
