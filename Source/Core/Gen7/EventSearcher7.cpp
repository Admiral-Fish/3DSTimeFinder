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

#include "EventSearcher7.hpp"
#include <Core/Parents/EventResult.hpp>
#include <Core/RNG/RNGList.hpp>
#include <Core/RNG/SFMT.hpp>
#include <Core/Util/PIDType.hpp>
#include <Core/Util/Utility.hpp>
#include <future>

EventSearcher7::EventSearcher7(const DateTime &startTime, const DateTime &endTime, u32 startFrame, u32 endFrame, u8 ivCount,
                               PIDType pidType, const Profile7 &profile, const EventFilter &filter) :
    profile(profile),
    filter(filter),
    startTime(startTime),
    endTime(endTime),
    startFrame(startFrame),
    endFrame(endFrame),
    ivCount(ivCount),
    pidType(pidType),
    progress(0),
    searching(false)
{
}

void EventSearcher7::setLocks(bool abilityLocked, u8 ability, bool natureLocked, u8 nature, bool genderLocked, u8 gender)
{
    this->abilityLocked = abilityLocked;
    this->ability = ability;
    this->natureLocked = natureLocked;
    this->nature = nature;
    this->genderLocked = genderLocked;
    this->gender = gender;
}

void EventSearcher7::setIDs(bool checkInfo, u16 tid, u16 sid, bool ownID)
{
    this->otherInfo = checkInfo;
    this->tid = tid;
    this->sid = sid;
    this->ownID = ownID;
}

void EventSearcher7::setHidden(u32 pid, u32 ec)
{
    this->pid = pid;
    this->ec = ec;
}

void EventSearcher7::setIVTemplate(const std::array<u8, 6> &ivs)
{
    ivTemplate = ivs;
}

void EventSearcher7::startSearch(int threads)
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

void EventSearcher7::cancelSearch()
{
    searching = false;
}

int EventSearcher7::getProgress() const
{
    return progress;
}

int EventSearcher7::getMaxProgress() const
{
    auto val = static_cast<int>(
        (Utility::getCitraTime(endTime, profile.getOffset()) - Utility::getCitraTime(startTime, profile.getOffset())) / 1000);
    return val + 1;
}

std::vector<EventResult> EventSearcher7::getResults()
{
    std::lock_guard<std::mutex> lock(mutex);
    auto data = std::move(results);
    return data;
}

void EventSearcher7::search(u64 epochStart, u64 epochEnd)
{
    u32 tick = profile.getTick();
    u32 offset = profile.getOffset();
    u16 eventTID = ownID ? profile.getTID() : tid;
    u16 eventSID = ownID ? profile.getSID() : sid;

    DateTime target = DateTime(Utility::getNormalTime(epochStart, offset));
    for (u64 epoch = epochStart; epoch <= epochEnd && searching; epoch += 1000, target.addSeconds(1))
    {
        u32 initialSeed = Utility::calcInitialSeed(tick, epoch);

        SFMT sfmt(initialSeed, startFrame);
        RNGList<u64, SFMT, 64> rngList(sfmt);

        for (u32 frame = startFrame; frame <= endFrame; frame++, rngList.advanceState())
        {
            EventResult result(initialSeed, eventTID, eventSID);

            result.setEC(ec > 0 ? ec : rngList.getValue() & 0xFFFFFFFF);

            switch (pidType)
            {
            case PIDType::Random:
                result.setPID(rngList.getValue() & 0xFFFFFFFF);
                break;
            case PIDType::Nonshiny:
                result.setPID(rngList.getValue() & 0xFFFFFFFF);
                if (result.getShiny())
                {
                    result.setPID(result.getPID() ^ 0x10000000);
                }
                break;
            case PIDType::Shiny:
                result.setPID(rngList.getValue() & 0xFFFFFFFF);
                if (otherInfo)
                {
                    result.setPID(((tid ^ sid ^ (result.getPID() & 0xFFFF)) << 16) | (result.getPID() & 0xFFFF));
                }
                break;
            case PIDType::Specified:
                result.setPID(pid);
                break;
            }

            result.setIVs(ivTemplate);
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
                    result.setIV(i, rngList.getValue() & 0x1F);
                }
            }
            result.calcHiddenPower();

            result.setAbility(abilityLocked ? ability : ability == 0 ? rngList.getValue() & 1 : rngList.getValue() % 3);

            result.setNature(natureLocked ? nature : rngList.getValue() % 25);

            result.setGender(genderLocked ? gender : (rngList.getValue() % 252) < gender);

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
