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

#include "IDSearcher7.hpp"
#include <Core/RNG/SFMT.hpp>
#include <Core/Util/Utility.hpp>
#include <QThreadPool>
#include <QtConcurrent>

IDSearcher7::IDSearcher7(const QDateTime &startTime, const QDateTime &endTime, u32 startFrame, u32 endFrame, const Profile7 &profile,
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
        (Utility::getCitraTime(endTime, profile.getOffset()) - Utility::getCitraTime(startTime, profile.getOffset()))
        / 1000);
    return val + 1;
}

QVector<IDResult> IDSearcher7::getResults()
{
    std::lock_guard<std::mutex> lock(resultMutex);

    auto data(results);
    results.clear();

    return data;
}

void IDSearcher7::search(u64 epochStart, u64 epochEnd)
{
    u32 tick = profile.getTick();
    u32 offset = profile.getOffset();

    for (u64 epoch = epochStart; epoch <= epochEnd && searching; epoch += 1000)
    {
        auto target = QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(Utility::getNormalTime(epoch, offset)), Qt::UTC);
        u32 initialSeed = Utility::calcInitialSeed(tick, epoch);

        SFMT sfmt(initialSeed, startFrame);

        for (u32 frame = startFrame; frame <= endFrame; frame++)
        {
            IDResult id(initialSeed, frame, sfmt.next() & 0xffffffff);
            if (filter.compare(id))
            {
                id.setTarget(target);

                std::lock_guard<std::mutex> lock(resultMutex);
                results.append(id);
            }
        }

        std::lock_guard<std::mutex> lock(progressMutex);
        progress++;
    }
}
