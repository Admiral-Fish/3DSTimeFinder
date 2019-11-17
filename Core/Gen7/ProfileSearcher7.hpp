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

#ifndef PROFILESEARCHER7_HPP
#define PROFILESEARCHER7_HPP

#include <Core/Global.hpp>
#include <QDateTime>
#include <QMutex>
#include <QObject>
#include <QVector>

class ProfileSearcher7 : public QObject
{
    Q_OBJECT
signals:
    void finished();
    void updateProgress(const QVector<QPair<u32, u32>> &results, int progress);

public:
    ProfileSearcher7(
        const QDateTime &start, u32 initialSeed, u32 baseTick, u32 baseOffset, u32 tickRange, u32 offsetRange);
    void startSearch();
    int maxProgress();

public slots:
    void cancelSearch();

private:
    QDateTime startDate;
    u32 initialSeed;
    u32 baseTick, baseOffset;
    u32 tickRange, offsetRange;
    QVector<QPair<u32, u32>> results;
    QMutex mutex;
    int progress;
    bool searching, cancel;

    void search();
    void update();
    QVector<QPair<u32, u32>> getResults();
};

#endif // PROFILESEARCHER7_HPP
