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

#ifndef IDSEARCHER7_HPP
#define IDSEARCHER7_HPP

#include <Core/Gen7/Profile7.hpp>
#include <Core/Parents/IDFilter.hpp>
#include <Core/Parents/IDResult.hpp>
#include <mutex>

class IDSearcher7
{
public:
    IDSearcher7(const QDateTime &startTime, const QDateTime &endTime, u32 startFrame, u32 endFrame, const Profile7 &profile,
                const IDFilter &filter);
    void startSearch(int threads);
    void cancelSearch();
    int getProgress() const;
    int getMaxProgress() const;
    std::vector<IDResult> getResults();

private:
    QDateTime startTime, endTime;
    u32 startFrame, endFrame;
    IDFilter filter;
    Profile7 profile;

    std::vector<IDResult> results;
    std::mutex resultMutex, progressMutex;
    int progress;
    bool searching;

    void search(u64 epochStart, u64 epochEnd);
};

#endif // IDSEARCHER7_HPP
