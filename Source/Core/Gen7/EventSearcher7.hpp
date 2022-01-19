/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2022 by Admiral_Fish
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
#include <Core/Util/DateTime.hpp>
#include <atomic>
#include <mutex>

class EventResult;
enum PIDType;

class EventSearcher7
{
public:
    EventSearcher7(const DateTime &startTime, const DateTime &endTime, u32 startFrame, u32 endFrame, u8 ivCount, PIDType pidType,
                   const Profile7 &profile, const EventFilter &filter);
    void setLocks(bool abilityLocked, u8 ability, bool natureLocked, u8 nature, bool genderLocked, u8 gender);
    void setIDs(bool checkInfo, u16 tid, u16 sid, bool ownID);
    void setHidden(u32 pid, u32 ec);
    void setIVTemplate(const std::array<u8, 6> &ivs);
    void startSearch(int threads);
    void cancelSearch();
    int getProgress() const;
    int getMaxProgress() const;
    std::vector<EventResult> getResults();

private:
    Profile7 profile;
    EventFilter filter;
    DateTime startTime, endTime;
    u32 startFrame, endFrame;
    u8 ivCount, ability, nature, gender;
    bool otherInfo, abilityLocked, natureLocked, genderLocked, ownID;
    PIDType pidType;
    u32 ec, pid;
    u16 tid, sid;
    std::array<u8, 6> ivTemplate;

    std::vector<EventResult> results;
    std::mutex mutex;
    std::atomic_int progress;
    bool searching;

    void search(u64 epochStart, u64 epochEnd);
};

#endif // EVENTSEARCHER7_HPP
