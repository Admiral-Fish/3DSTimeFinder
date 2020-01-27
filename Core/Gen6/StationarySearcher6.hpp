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

#ifndef STATIONARYSEARCHER6_HPP
#define STATIONARYSEARCHER6_HPP

#include <Core/Gen6/Profile6.hpp>
#include <Core/Parents/StationaryFilter.hpp>
#include <Core/Parents/StationaryResult.hpp>
#include <QVector>
#include <mutex>

class StationarySearcher6
{
public:
    StationarySearcher6(const QDateTime &startTime, const QDateTime &endTime, u32 startFrame, u32 endFrame, bool ivCount, u8 ability,
                        u8 synchNature, u8 gender, bool alwaysSynch, bool shinyLocked, const Profile6 &profile,
                        const StationaryFilter &filter);
    void startSearch(int threads);
    void cancelSearch();
    int getProgress() const;
    int getMaxProgress() const;
    QVector<StationaryResult> getResults();

private:
    Profile6 profile;
    StationaryFilter filter;
    QDateTime startTime, endTime;
    u32 startFrame, endFrame;
    u8 ivCount, ability, synchNature, pidCount, gender;
    bool alwaysSynch, shinyLocked;

    QVector<StationaryResult> results;
    std::mutex resultMutex, progressMutex;
    int progress;
    bool searching;

    void search(u64 epochStart, u64 epochEnd);
};

#endif // STATIONARYSEARCHER6_HPP
