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

#ifndef EVENTSEARCHER7_HPP
#define EVENTSEARCHER7_HPP

#include <Core/Gen7/Profile7.hpp>
#include <Core/Parents/EventFilter.hpp>
#include <Core/Parents/EventResult.hpp>
#include <Core/Util/PIDType.hpp>
#include <QMutex>
#include <QObject>
#include <QThreadPool>
#include <QVector>

class EventSearcher7 : public QObject
{
    Q_OBJECT
signals:
    void finished();
    void threadFinished();
    void updateProgress(const QVector<EventResult> &results, int progress);

public:
    EventSearcher7(const QDateTime &start, const QDateTime &end, u32 startFrame, u32 endFrame, int ivCount,
        PIDType pidType, const Profile7 &profile, const EventFilter &filter);
    void setLocks(bool abilityLocked, int ability, bool natureLocked, int nature, bool genderLocked, int gender);
    void setIDs(bool checkInfo, u16 tid, u16 sid, bool ownID);
    void setHidden(u32 pid, u32 ec);
    void setIVTemplate(const QVector<u8> &ivs);
    void startSearch();
    int maxProgress();

public slots:
    void cancelSearch();

private:
    Profile7 profile;
    EventFilter filter;
    QDateTime startTime, endTime;
    u32 startFrame, endFrame;
    int ivCount, ability, nature, gender;
    bool otherInfo, abilityLocked, natureLocked, genderLocked, ownID;
    PIDType pidType;
    u32 ec, pid;
    u16 tid, sid;
    QVector<u8> ivTemplate;

    QVector<EventResult> results;
    QThreadPool threadPool;
    QMutex resultMutex, threadMutex;
    int progress, threads, threadsFinished;
    bool searching, cancel;

    void search(u64 epochStart, u64 epochEnd);
    void update();
    QVector<EventResult> getResults();

private slots:
    void checkFinish();
};

#endif // EVENTSEARCHER7_HPP
