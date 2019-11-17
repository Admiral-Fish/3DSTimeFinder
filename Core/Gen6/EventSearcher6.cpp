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

#include "EventSearcher6.hpp"
#include <Core/RNG/MT.hpp>
#include <Core/Util/Utility.hpp>
#include <QtConcurrent>

EventSearcher6::EventSearcher6(const QDateTime &start, const QDateTime &end, u32 startFrame, u32 endFrame, int ivCount,
    PIDType pidType, const Profile6 &profile, const EventFilter &filter)
{
    startTime = start;
    endTime = end;
    this->startFrame = startFrame;
    this->endFrame = endFrame;
    this->ivCount = ivCount;
    this->pidType = pidType;
    this->profile = profile;
    this->filter = filter;

    searching = false;
    cancel = false;
    progress = 0;
    threadsFinished = 0;

    connect(this, &EventSearcher6::threadFinished, this, &EventSearcher6::checkFinish);
    connect(this, &EventSearcher6::finished, this, [=] {
        searching = false;
        emit updateProgress(getResults(), progress);
        QTimer::singleShot(1000, this, &EventSearcher6::deleteLater);
    });
}

void EventSearcher6::setLocks(
    bool abilityLocked, int ability, bool natureLocked, int nature, bool genderLocked, int gender)
{
    this->abilityLocked = abilityLocked;
    this->ability = ability;
    this->natureLocked = natureLocked;
    this->nature = nature;
    this->genderLocked = genderLocked;
    this->gender = gender;
}

void EventSearcher6::setIDs(bool checkInfo, u16 tid, u16 sid, bool ownID)
{
    this->otherInfo = checkInfo;
    this->tid = tid;
    this->sid = sid;
    this->ownID = ownID;
}

void EventSearcher6::setHidden(u32 pid, u32 ec)
{
    this->pid = pid;
    this->ec = ec;
}

void EventSearcher6::setIVTemplate(const QVector<u8> &ivs)
{
    ivTemplate = ivs;
}

void EventSearcher6::startSearch()
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

int EventSearcher6::maxProgress()
{
    auto val = static_cast<int>((Utility::getCitraTime(endTime) - Utility::getCitraTime(startTime)) / 1000);
    return val + 1;
}

void EventSearcher6::cancelSearch()
{
    cancel = true;
}

void EventSearcher6::search(u64 epochStart, u64 epochEnd)
{
    QVector<u32> rngList(endFrame - startFrame + 2000);
    u32 save = profile.getSaveVariable();
    u32 time = profile.getTimeVariable();
    u16 eventTID = ownID ? profile.getTID() : tid;
    u16 eventSID = ownID ? profile.getSID() : sid;
    u8 counter = (profile.getVersion() & Game::ORAS) ? 2 : 1;

    for (u64 epoch = epochStart; epoch <= epochEnd; epoch += 1000)
    {
        if (cancel)
        {
            emit threadFinished();
            return;
        }

        QDateTime target
            = QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(Utility::getNormalTime(epoch)), Qt::UTC);
        u32 initialSeed = save + time + epoch;
        MT mt(initialSeed, startFrame);

        for (u32 &x : rngList)
        {
            x = mt.nextUInt();
        }

        for (u32 frame = 0; frame <= (endFrame - startFrame); frame++)
        {
            EventResult result(initialSeed, eventTID, eventSID);
            u32 index = 1;

            for (u8 j = 0; j < counter; j++)
            {
                result.setEC(ec > 0 ? ec : rngList.at(frame + index++));

                switch (pidType)
                {
                case PIDType::Random:
                    result.setPID(rngList.at(frame + index++));
                    break;
                case PIDType::Nonshiny:
                    result.setPID(rngList.at(frame + index++));
                    if (result.getShiny())
                    {
                        result.setPID(result.getPID() ^ 0x10000000);
                    }
                    break;
                case PIDType::Shiny:
                    result.setPID(rngList.at(frame + index++));
                    result.setShiny(true);
                    if (otherInfo)
                    {
                        result.setPID(((tid ^ sid ^ (result.getPID() & 0xFFFF)) << 16) | (result.getPID() & 0xFFFF));
                    }
                    break;
                case PIDType::Specified:
                    result.setPID(pid);
                    break;
                }

                result.setIVs(ivTemplate);
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

                result.setAbility(abilityLocked
                        ? ability
                        : ((static_cast<u64>(rngList.at(frame + index++)) * (ability + 2) >> 32) + 1));

                result.setNature(natureLocked ? nature : static_cast<u64>(rngList.at(frame + index++)) * 25 >> 32);

                result.setGender(genderLocked
                        ? gender
                        : (static_cast<u64>(rngList.at(frame + index++)) * 252 >> 32) >= gender ? 1 : 2);
            }

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

void EventSearcher6::update()
{
    do
    {
        emit updateProgress(getResults(), progress);
        QThread::sleep(1);
    } while (searching);
}

QVector<EventResult> EventSearcher6::getResults()
{
    QMutexLocker locker(&resultMutex);
    auto data(results);
    results.clear();

    return data;
}

void EventSearcher6::checkFinish()
{
    QMutexLocker locker(&threadMutex);
    if (++threadsFinished == threads)
    {
        emit finished();
    }
}
