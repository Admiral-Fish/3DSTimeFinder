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

#ifndef IDSEARCHER7_HPP
#define IDSEARCHER7_HPP

#include <Core/Gen7/Profile7.hpp>
#include <Core/Parents/IDFilter.hpp>
#include <Core/Parents/IDResult.hpp>
#include <QMutex>
#include <QObject>
#include <QThreadPool>
#include <QVector>

class IDSearcher7 : public QObject
{
    Q_OBJECT
signals:
    void finished();
    void threadFinished();
    void updateProgress(const QVector<IDResult> &results, int progress);

public:
    IDSearcher7(const QDateTime &start, const QDateTime &end, u32 startFrame, u32 endFrame, const Profile7 &profile,
        const IDFilter &filter);
    void startSearch();
    int maxProgress();

public slots:
    void cancelSearch();

private:
    QDateTime startTime, endTime;
    u32 startFrame, endFrame;
    IDFilter filter;
    Profile7 profile;

    QVector<IDResult> results;
    QThreadPool threadPool;
    QMutex resultMutex, threadMutex;
    int progress, threads, threadsFinished;
    bool searching, cancel;

    void search(u64 epochStart, u64 epochEnd);
    void update();
    QVector<IDResult> getResults();

private slots:
    void checkFinish();
};

#endif // IDSEARCHER7_HPP
