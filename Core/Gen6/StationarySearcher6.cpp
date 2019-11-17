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

#include "StationarySearcher6.hpp"
#include <Core/RNG/MT.hpp>
#include <Core/Util/Utility.hpp>
#include <QtConcurrent>

StationarySearcher6::StationarySearcher6(const QDateTime &start, const QDateTime &end, u32 startFrame, u32 endFrame,
    bool ivCount, int ability, int synchNature, int gender, bool alwaysSynch, bool shinyLocked, const Profile6 &profile,
    const StationaryFilter &filter)
{
    startTime = start;
    endTime = end;
    this->startFrame = startFrame;
    this->endFrame = endFrame;
    this->ivCount = ivCount ? 3 : 0;
    this->ability = ability;
    this->synchNature = synchNature;
    this->gender = gender;
    this->alwaysSynch = alwaysSynch;
    this->shinyLocked = shinyLocked;
    this->profile = profile;
    this->filter = filter;
    pidCount = profile.getShinyCharm() ? 3 : 1;

    searching = false;
    cancel = false;
    progress = 0;
    threadsFinished = 0;

    connect(this, &StationarySearcher6::threadFinished, this, &StationarySearcher6::checkFinish);
    connect(this, &StationarySearcher6::finished, this, [=] {
        searching = false;
        emit updateProgress(getResults(), progress);
        QTimer::singleShot(1000, this, &StationarySearcher6::deleteLater);
    });
}

void StationarySearcher6::startSearch()
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

        u64 epochStart = Utility::getCitraTime(startTime);
        u64 epochEnd = Utility::getCitraTime(endTime);

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

int StationarySearcher6::maxProgress()
{
    auto val = static_cast<int>((Utility::getCitraTime(endTime) - Utility::getCitraTime(startTime)) / 1000);
    return val + 1;
}

void StationarySearcher6::cancelSearch()
{
    cancel = true;
}

void StationarySearcher6::search(u64 epochStart, u64 epochEnd)
{
    QVector<u32> rngList(endFrame - startFrame + 120);
    u32 saveVariable = profile.getTimeVariable();
    u32 timeVariable = profile.getSaveVariable();
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
            = QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(Utility::getNormalTime(epoch)), Qt::UTC);
        u32 initialSeed = static_cast<u32>(saveVariable + epoch + timeVariable);
        MT mt(initialSeed, startFrame);

        for (u32 &x : rngList)
        {
            x = mt.nextUInt();
        }

        for (u32 frame = 0; frame <= (endFrame - startFrame); frame++)
        {
            StationaryResult result(initialSeed, tid, sid);
            u32 index = 1;

            if (!alwaysSynch)
            {
                index += 60;
            }

            result.setEC(rngList.at(frame + index++));

            for (u8 i = 0; i < pidCount; i++)
            {
                result.setPID(rngList.at(frame + index++));
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
                u8 tmp = static_cast<u64>(rngList.at(frame + index++)) * 6 >> 32;
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
                    result.setIV(i, rngList.at(frame + index++) >> 27);
                }
            }
            result.calcHiddenPower();

            result.setAbility(ability > 0 ? ability : ((rngList.at(frame + index++) >> 31) + 1));

            result.setNature(alwaysSynch ? synchNature : static_cast<u64>(rngList.at(frame + index++)) * 25 >> 32);

            result.setGender(
                gender > 2 ? (static_cast<u64>(rngList.at(frame + index++)) * 252 >> 32 >= gender ? 1 : 2) : gender);

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

void StationarySearcher6::update()
{
    do
    {
        emit updateProgress(getResults(), progress);
        QThread::sleep(1);
    } while (searching);
}

QVector<StationaryResult> StationarySearcher6::getResults()
{
    QMutexLocker locker(&resultMutex);
    auto data(results);
    results.clear();

    return data;
}

void StationarySearcher6::checkFinish()
{
    QMutexLocker locker(&threadMutex);
    if (++threadsFinished == threads)
    {
        emit finished();
    }
}
