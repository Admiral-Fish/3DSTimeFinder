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

#include "WildSearcher7.hpp"
#include <Core/RNG/RNGList.hpp>
#include <Core/RNG/SFMT.hpp>
#include <Core/Util/Utility.hpp>
#include <QThreadPool>
#include <QtConcurrent>

constexpr u8 grassSlots[10] = { 19, 39, 49, 59, 69, 79, 89, 94, 98, 99 };
constexpr u8 waterSlots[3] = { 78, 98, 99 };

WildSearcher7::WildSearcher7(const QDateTime &startTime, const QDateTime &endTime, u32 startFrame, u32 endFrame, bool useSynch,
                             u8 synchNature, WildType type, u8 gender, const Profile7 &profile, const WildFilter &filter) :
    profile(profile),
    filter(filter),
    startTime(startTime),
    endTime(endTime),
    startFrame(startFrame),
    endFrame(endFrame),
    synchNature(synchNature),
    pidCount(profile.getShinyCharm() ? 3 : 1),
    gender(gender),
    useSynch(useSynch),
    type(type),
    progress(0),
    searching(false)
{
}

void WildSearcher7::startSearch(int threads)
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

void WildSearcher7::cancelSearch()
{
    searching = false;
}

int WildSearcher7::getProgress() const
{
    return progress;
}

int WildSearcher7::getMaxProgress() const
{
    auto val = static_cast<int>(
        (Utility::getCitraTime(endTime, profile.getOffset()) - Utility::getCitraTime(startTime, profile.getOffset()))
        / 1000);
    return val + 1;
}

QVector<WildResult> WildSearcher7::getResults()
{
    std::lock_guard<std::mutex> lock(resultMutex);

    auto data(results);
    results.clear();

    return data;
}

void WildSearcher7::search(u64 epochStart, u64 epochEnd)
{
    u32 tick = profile.getTick();
    u32 offset = profile.getOffset();
    u16 tid = profile.getTID();
    u16 sid = profile.getSID();

    for (u64 epoch = epochStart; epoch <= epochEnd && searching; epoch += 1000)
    {
        auto target = QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(Utility::getNormalTime(epoch, offset)), Qt::UTC);
        u32 initialSeed = Utility::calcInitialSeed(tick, epoch);

        SFMT sfmt(initialSeed, startFrame);
        RNGList<u64, SFMT, 128> rngList(sfmt);

        for (u32 frame = startFrame; frame <= endFrame; frame++, rngList.advanceState())
        {
            WildResult result(initialSeed, tid, sid);

            // Lead eats a call
            bool synch = (rngList.getValue() % 100 >= 50) && useSynch;

            result.setEncounterSlot(getSlot(rngList.getValue() % 100));

            // Level eats a call
            rngList.advanceFrames(1);

            // Flute eats a call
            rngList.advanceFrames(1);

            // If Minior eat a call, but I'm going to ignore this

            // Advance(60)
            rngList.advanceFrames(60);

            result.setEC(rngList.getValue() & 0xFFFFFFFF);

            for (u8 i = 0; i < pidCount; i++)
            {
                result.setPID(rngList.getValue() & 0xffffffff);
                if (result.getShiny())
                {
                    break;
                }
            }

            for (u8 i = 0; i < 6; i++)
            {
                result.setIV(i, rngList.getValue() & 0x1f);
            }
            result.calcHiddenPower();

            result.setAbility(rngList.getValue() & 1);

            result.setNature(synch ? synchNature : rngList.getValue() % 25);

            // This might be wrong, it's probably fine though
            result.setGender((gender > 0 && gender < 254) ? (rngList.getValue() % 252 >= gender ? 1 : 2) : gender);

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

u8 WildSearcher7::getSlot(u8 value)
{
    if (type == WildType::Grass)
    {
        for (u8 i = 0; i < 10; i++)
        {
            if (value <= grassSlots[i])
            {
                return i + 1;
            }
        }
        return 255;
    }

    for (u8 i = 0; i < 3; i++)
    {
        if (value <= waterSlots[i])
        {
            return i + 1;
        }
    }
    return 255;
}
