/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019 by Admiral_Fish
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
#include <Core/RNG/SFMT.hpp>
#include <Core/Util/Utility.hpp>
#include <QtConcurrent>

constexpr u8 grassSlots[10] = { 19, 39, 49, 59, 69, 79, 89, 94, 98, 99 };
constexpr u8 waterSlots[3] = { 78, 98, 99 };

WildSearcher7::WildSearcher7(const QDateTime &start, const QDateTime &end, u32 startFrame, u32 endFrame, bool useSynch,
    int synchNature, WildType type, int gender, const Profile7 &profile, const WildFilter &filter)
{
    startTime = start;
    endTime = end;
    this->startFrame = startFrame;
    this->endFrame = endFrame;
    this->useSynch = useSynch;
    this->synchNature = synchNature;
    this->type = type;
    this->gender = gender;
    this->profile = profile;
    this->filter = filter;
    pidCount = profile.getShinyCharm() ? 3 : 1;

    searching = false;
    cancel = false;
    progress = 0;
    threadsFinished = 0;

    connect(this, &WildSearcher7::threadFinished, this, &WildSearcher7::checkFinish);
    connect(this, &WildSearcher7::finished, this, [=] {
        searching = false;
        emit updateProgress(getResults(), progress);
        QTimer::singleShot(1000, this, &WildSearcher7::deleteLater);
    });
}

void WildSearcher7::startSearch()
{
    if (!searching)
    {
        searching = true;
        cancel = false;
        progress = 0;
        threadsFinished = 0;

        QSettings setting;
        threads = setting.value("settings/threads", QThread::idealThreadCount()).toInt();
        threadPool.setMaxThreadCount(threads + 1);

        u64 epochStart = Utility::getCitraTime(startTime, profile.getOffset());
        u64 epochEnd = Utility::getCitraTime(endTime, profile.getOffset());

        u64 epochSplit = (epochEnd - epochStart) / threads;
        epochSplit -= (epochSplit % 1000); // Floor to nearest thousand

        QtConcurrent::run(&threadPool, [=] { update(); });
        for (int i = 0; i < threads; i++)
        {
            if (i == threads - 1)
            {
                QtConcurrent::run(&threadPool, [=] { search(epochStart, epochEnd); });
            }
            else
            {
                QtConcurrent::run(&threadPool, [=] { search(epochStart, epochStart + epochSplit); });
            }
            epochStart += epochSplit;
        }
    }
}

int WildSearcher7::maxProgress()
{
    auto val = static_cast<int>(
        (Utility::getCitraTime(endTime, profile.getOffset()) - Utility::getCitraTime(startTime, profile.getOffset()))
        / 1000);
    return val + 1;
}

void WildSearcher7::cancelSearch()
{
    cancel = true;
}

void WildSearcher7::search(u64 epochStart, u64 epochEnd)
{
    QVector<u64> rngList(endFrame - startFrame + 110);
    u32 tick = profile.getTick();
    u32 offset = profile.getOffset();
    u16 tid = profile.getTID();
    u16 sid = profile.getSID();

    for (u64 epoch = epochStart; epoch <= epochEnd; epoch += 1000)
    {
        if (cancel)
        {
            emit threadFinished();
            return;
        }

        QDateTime target
            = QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(Utility::getNormalTime(epoch, offset)), Qt::UTC);
        u32 initialSeed = Utility::calcInitialSeed(tick, epoch);
        SFMT sfmt(initialSeed);
        sfmt.advanceFrames(startFrame);

        for (u64 &x : rngList)
        {
            x = sfmt.nextULong();
        }

        for (u32 frame = 0; frame <= (endFrame - startFrame); frame++)
        {
            WildResult result(initialSeed, tid, sid);
            u32 index = 0;

            // Lead eats a call
            bool synch = (rngList.at(frame + index++) % 100 >= 50) && useSynch;

            result.setEncounterSlot(getSlot(rngList.at(frame + index++) % 100));

            // Level eats a call
            index++;

            // Flute eats a call
            index++;

            // If Minior eat a call, but I'm going to ignore this

            // Advance(60)
            index += 60;

            result.setEC(rngList.at(frame + index++) & 0xFFFFFFFF);

            for (u8 i = 0; i < pidCount; i++)
            {
                result.setPID(rngList.at(frame + index++) & 0xffffffff);
                if (result.getShiny())
                {
                    break;
                }
            }

            for (u8 i = 0; i < 6; i++)
            {
                result.setIV(i, rngList.at(frame + index++) & 0x1f);
            }
            result.calcHiddenPower();

            result.setAbility((rngList.at(frame + index++) & 1) + 1);

            result.setNature(synch ? synchNature : rngList.at(frame + index++) % 25);

            // This might be wrong, it's probably fine though
            result.setGender(gender > 2 ? (rngList.at(frame + index++) % 252 >= gender ? 1 : 2) : gender);

            if (filter.compare(result))
            {
                result.setTarget(target);
                result.setFrame(frame + startFrame);

                QMutexLocker locker(&resultMutex);
                results.append(result);
            }
        }

        progress++;
    }
    emit threadFinished();
}

void WildSearcher7::update()
{
    do
    {
        emit updateProgress(getResults(), progress);
        QThread::sleep(1);
    } while (searching);
}

QVector<WildResult> WildSearcher7::getResults()
{
    QMutexLocker locker(&resultMutex);
    auto data(results);
    results.clear();

    return data;
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
        return -1;
    }

    for (u8 i = 0; i < 3; i++)
    {
        if (value <= waterSlots[i])
        {
            return i + 1;
        }
    }
    return -1;
}

void WildSearcher7::checkFinish()
{
    QMutexLocker locker(&threadMutex);
    if (++threadsFinished == threads)
    {
        searching = false;
        emit finished();
    }
}
