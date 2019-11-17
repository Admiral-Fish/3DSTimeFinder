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

#include "IDSearcher7.hpp"
#include <Core/RNG/SFMT.hpp>
#include <Core/Util/Utility.hpp>
#include <QtConcurrent>

IDSearcher7::IDSearcher7(const QDateTime &start, const QDateTime &end, u32 startFrame, u32 endFrame,
    const Profile7 &profile, const IDFilter &filter)
{
    startTime = start;
    endTime = end;
    this->startFrame = startFrame;
    this->endFrame = endFrame;
    this->profile = profile;
    this->filter = filter;

    searching = false;
    cancel = false;
    progress = 0;
    threadsFinished = 0;

    connect(this, &IDSearcher7::threadFinished, this, &IDSearcher7::checkFinish);
    connect(this, &IDSearcher7::finished, this, [=] {
        searching = false;
        emit updateProgress(getResults(), progress);
        QTimer::singleShot(1000, this, &IDSearcher7::deleteLater);
    });
}

void IDSearcher7::startSearch()
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

int IDSearcher7::maxProgress()
{
    auto val = static_cast<int>(
        (Utility::getCitraTime(endTime, profile.getOffset()) - Utility::getCitraTime(startTime, profile.getOffset()))
        / 1000);
    return val + 1;
}

void IDSearcher7::cancelSearch()
{
    cancel = true;
}

void IDSearcher7::search(u64 epochStart, u64 epochEnd)
{
    u32 tick = profile.getTick();
    u32 offset = profile.getOffset();

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
        SFMT sfmt(initialSeed, startFrame);

        for (u32 frame = startFrame; frame <= endFrame; frame++)
        {
            IDResult id(initialSeed, frame, sfmt.nextULong() & 0xffffffff);
            if (filter.compare(id))
            {
                id.setTarget(target);

                QMutexLocker locker(&resultMutex);
                results.append(id);
            }
        }

        progress++;
    }
    emit threadFinished();
}

void IDSearcher7::update()
{
    do
    {
        emit updateProgress(getResults(), progress);
        QThread::sleep(1);
    } while (searching);
}

QVector<IDResult> IDSearcher7::getResults()
{
    QMutexLocker locker(&resultMutex);
    auto data(results);
    results.clear();

    return data;
}

void IDSearcher7::checkFinish()
{
    QMutexLocker locker(&threadMutex);
    if (++threadsFinished == threads)
    {
        emit finished();
    }
}
