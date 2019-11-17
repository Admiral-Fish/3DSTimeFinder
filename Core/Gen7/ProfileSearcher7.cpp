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

#include "ProfileSearcher7.hpp"
#include <Core/Util/Utility.hpp>
#include <QtConcurrent>

ProfileSearcher7::ProfileSearcher7(
    const QDateTime &start, u32 initialSeed, u32 baseTick, u32 baseOffset, u32 tickRange, u32 offsetRange)
{
    startDate = start;
    this->initialSeed = initialSeed;
    this->baseTick = baseTick;
    this->baseOffset = baseOffset;
    this->tickRange = tickRange;
    this->offsetRange = offsetRange;

    searching = false;
    cancel = false;
    progress = 0;

    connect(this, &ProfileSearcher7::finished, this, [=] {
        searching = false;
        emit updateProgress(getResults(), progress);
        QTimer::singleShot(1000, this, &ProfileSearcher7::deleteLater);
    });
}

void ProfileSearcher7::startSearch()
{
    if (!searching)
    {
        searching = true;
        cancel = false;
        progress = 0;

        QtConcurrent::run([=] { update(); });
        QtConcurrent::run([=] { search(); });
    }
}

int ProfileSearcher7::maxProgress()
{
    return static_cast<int>(tickRange + 1);
}

void ProfileSearcher7::cancelSearch()
{
    cancel = true;
}

void ProfileSearcher7::search()
{
    for (u32 tick = 0; tick <= tickRange; tick++)
    {
        for (u32 offset = 0; offset <= offsetRange; offset++)
        {
            if (cancel)
            {
                emit finished();
                return;
            }

            // Plus offset
            u64 epochPlus = Utility::getCitraTime(startDate, baseOffset + offset);
            u32 seedPlus = Utility::calcInitialSeed(baseTick + tick, epochPlus);
            if (seedPlus == initialSeed)
            {
                QMutexLocker locker(&mutex);
                results.append(QPair<u32, u32>(baseTick + tick, baseOffset + offset));
            }

            // Minus offset
            u64 epochMinus = Utility::getCitraTime(startDate, baseOffset - offset);
            u32 seedMinus = Utility::calcInitialSeed(baseTick - tick, epochMinus);
            if (seedMinus == initialSeed)
            {
                QMutexLocker locker(&mutex);
                results.append(QPair<u32, u32>(baseTick - tick, baseOffset - offset));
            }
        }
        progress++;
    }
    emit finished();
}

void ProfileSearcher7::update()
{
    do
    {
        emit updateProgress(getResults(), progress);
        QThread::sleep(1);
    } while (searching);
}

QVector<QPair<u32, u32>> ProfileSearcher7::getResults()
{
    QMutexLocker locker(&mutex);
    auto data(results);
    results.clear();

    return data;
}
