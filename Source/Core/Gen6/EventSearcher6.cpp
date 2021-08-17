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

#include "EventSearcher6.hpp"
#include <Core/RNG/MT.hpp>
#include <Core/RNG/RNGList.hpp>
#include <Core/Util/Utility.hpp>
#include <future>

EventSearcher6::EventSearcher6(const DateTime &startTime, const DateTime &endTime, u32 startFrame, u32 endFrame, u8 ivCount,
                               PIDType pidType, const Profile6 &profile, const EventFilter &filter) :
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

void EventSearcher6::setLocks(bool abilityLocked, u8 ability, bool natureLocked, u8 nature, bool genderLocked, u8 gender)
{
    this->abilityLocked = abilityLocked;
    this->ability = ability;
    this->natureLocked = natureLocked;
    this->nature = nature;
    this->genderLocked = genderLocked;
    this->gender = gender;
}

void EventSearcher6::setIDs(bool checkInfo, u16 tid, u16 sid, bool ownID)
{
    this->otherInfo = checkInfo;
    this->tid = tid;
    this->sid = sid;
    this->ownID = ownID;
}

void EventSearcher6::setHidden(u32 pid, u32 ec)
{
    this->pid = pid;
    this->ec = ec;
}

void EventSearcher6::setIVTemplate(const std::array<u8, 6> &ivs)
{
    ivTemplate = ivs;
}

void EventSearcher6::startSearch(int threads)
{
    searching = true;

    u64 epochStart = Utility::getCitraTime(startTime);
    u64 epochEnd = Utility::getCitraTime(endTime);

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

void EventSearcher6::cancelSearch()
{
    searching = false;
}

int EventSearcher6::getProgress() const
{
    return progress;
}

int EventSearcher6::getMaxProgress() const
{
    auto val = static_cast<int>((Utility::getCitraTime(endTime) - Utility::getCitraTime(startTime)) / 1000);
    return val + 1;
}

std::vector<EventResult> EventSearcher6::getResults()
{
    std::lock_guard<std::mutex> lock(mutex);
    auto data = std::move(results);
    return data;
}

void EventSearcher6::search(u64 epochStart, u64 epochEnd)
{
    u32 save = profile.getSaveVariable();
    u32 time = profile.getTimeVariable();
    u16 eventTID = ownID ? profile.getTID() : tid;
    u16 eventSID = ownID ? profile.getSID() : sid;
    u8 counter = (profile.getVersion() & Game::ORAS) ? 2 : 1;

    DateTime target = DateTime(Utility::getNormalTime(epochStart));
    for (u64 epoch = epochStart; epoch <= epochEnd && searching; epoch += 1000, target.addSeconds(1))
    {
        u32 initialSeed = save + time + epoch;

        MT mt(initialSeed, startFrame);
        RNGList<u32, MT, 128> rngList(mt);

        for (u32 frame = startFrame; frame <= endFrame; frame++, rngList.advanceState())
        {
            EventResult result(initialSeed, eventTID, eventSID);

            for (u8 j = 0; j < counter; j++)
            {
                result.setEC(ec > 0 ? ec : rngList.getValue());

                switch (pidType)
                {
                case PIDType::Random:
                    result.setPID(rngList.getValue());
                    break;
                case PIDType::Nonshiny:
                    result.setPID(rngList.getValue());
                    if (result.getShiny())
                    {
                        result.setPID(result.getPID() ^ 0x10000000);
                    }
                    break;
                case PIDType::Shiny:
                    result.setPID(rngList.getValue());
                    result.setShiny(true);
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
                    u8 tmp = static_cast<u64>(rngList.getValue()) * 6 >> 32;
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
                        result.setIV(i, rngList.getValue() >> 27);
                    }
                }
                result.calcHiddenPower();

                result.setAbility(abilityLocked ? ability : (static_cast<u64>(rngList.getValue()) * (ability + 2) >> 32));

                result.setNature(natureLocked ? nature : static_cast<u64>(rngList.getValue()) * 25 >> 32);

                result.setGender(genderLocked ? gender : (static_cast<u64>(rngList.getValue()) * 252 >> 32) < gender);
            }

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
