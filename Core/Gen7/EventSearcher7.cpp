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

#include "EventSearcher7.hpp"
#include <Core/RNG/RNGList.hpp>
#include <Core/RNG/SFMT.hpp>
#include <Core/Util/Utility.hpp>
#include <QThreadPool>
#include <QtConcurrent>

EventSearcher7::EventSearcher7(const QDateTime &startTime, const QDateTime &endTime, u32 startFrame, u32 endFrame, u8 ivCount,
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

void EventSearcher7::setIVTemplate(const QVector<u8> &ivs)
{
    ivTemplate = ivs;
}

void EventSearcher7::startSearch(int threads)
{
    searching = true;
    QThreadPool pool;

    u64 epochStart = Utility::getCitraTime(startTime, profile.getOffset());
    u64 epochEnd = Utility::getCitraTime(endTime, profile.getOffset());

    u64 epochSplit = (epochEnd - epochStart) / threads;
    epochSplit -= (epochSplit % 1000); // Floor to nearest thousand

    if (epochSplit < 1000)
    {
        pool.setMaxThreadCount(1);
        auto future = QtConcurrent::run(&pool, [=] { search(epochStart, epochEnd); });
        future.waitForFinished();

        return;
    }

    pool.setMaxThreadCount(threads);
    QVector<QFuture<void>> threadContainer;
    for (int i = 0; i < threads; i++)
    {
        if (i == threads - 1)
        {
            threadContainer.append(QtConcurrent::run(&pool, [=] { search(epochStart, epochEnd); }));
        }
        else
        {
            threadContainer.append(QtConcurrent::run(&pool, [=] { search(epochStart, epochStart + epochSplit); }));
        }
        epochStart += epochSplit;
    }

    for (int i = 0; i < threads; i++)
    {
        threadContainer[i].waitForFinished();
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
        (Utility::getCitraTime(endTime, profile.getOffset()) - Utility::getCitraTime(startTime, profile.getOffset()))
        / 1000);
    return val + 1;
}

QVector<EventResult> EventSearcher7::getResults()
{
    std::lock_guard<std::mutex> lock(resultMutex);

    auto data(results);
    results.clear();

    return data;
}

void EventSearcher7::search(u64 epochStart, u64 epochEnd)
{
    u32 tick = profile.getTick();
    u32 offset = profile.getOffset();
    u16 eventTID = ownID ? profile.getTID() : tid;
    u16 eventSID = ownID ? profile.getSID() : sid;

    for (u64 epoch = epochStart; epoch <= epochEnd && searching; epoch += 1000)
    {
        auto target = QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(Utility::getNormalTime(epoch, offset)), Qt::UTC);
        u32 initialSeed = Utility::calcInitialSeed(tick, epoch);

        SFMT sfmt(initialSeed, startFrame);
        RNGList<u64, SFMT, 50> rngList(sfmt);

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

                std::lock_guard<std::mutex> lock(resultMutex);
                results.append(result);
            }
        }

        std::lock_guard<std::mutex> lock(progressMutex);
        progress++;
    }
}
