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

#include "Profile6.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>
#include <QVector>

Profile6::Profile6()
    : Profile()
{
    saveVariable = 0;
    timeVariable = 0;
}

Profile6::Profile6(
    const QString &name, u32 saveVariable, u32 timeVariable, u16 tid, u16 sid, Game version, bool shinyCharm)
    : Profile(name, tid, sid, version, shinyCharm)
{
    this->saveVariable = saveVariable;
    this->timeVariable = timeVariable;
}

Profile6::Profile6(QJsonObject data)
    : Profile(data["name"].toString(), data["tid"].toInt(), data["sid"].toInt(),
        static_cast<Game>(data["version"].toInt()), data["charm"].toBool())
{
    saveVariable = data["save"].toString().toUInt(nullptr, 16);
    timeVariable = data["time"].toString().toUInt(nullptr, 16);
}

QJsonObject Profile6::getJson()
{
    QJsonObject data;
    data["name"] = name;
    data["save"] = QString::number(saveVariable, 16);
    data["time"] = QString::number(timeVariable, 16);
    data["tid"] = tid;
    data["sid"] = sid;
    data["version"] = version;
    data["charm"] = shinyCharm;
    return data;
}

u32 Profile6::getSaveVariable() const
{
    return saveVariable;
}

u32 Profile6::getTimeVariable() const
{
    return timeVariable;
}

QVector<Profile6> Profile6::loadProfileList()
{
    QVector<Profile6> profileList;
    QSettings setting;

    QJsonObject profiles(QJsonDocument::fromJson(setting.value("profiles").toByteArray()).object());
    QJsonArray gen6 = profiles["gen6"].toArray();

    for (const auto &&i : gen6)
    {
        auto data = i.toObject();
        profileList.append(Profile6(data));
    }

    return profileList;
}

void Profile6::saveProfile()
{
    QSettings setting;

    QJsonObject profiles(QJsonDocument::fromJson(setting.value("profiles").toByteArray()).object());
    QJsonArray gen6 = profiles["gen6"].toArray();

    gen6.append(getJson());
    profiles["gen6"] = gen6;

    setting.setValue("profiles", QJsonDocument(profiles).toJson());
}

void Profile6::deleteProfile()
{
    QSettings setting;

    QJsonObject profiles(QJsonDocument::fromJson(setting.value("profiles").toByteArray()).object());
    QJsonArray gen6 = profiles["gen6"].toArray();

    for (u8 i = 0; i < gen6.size(); i++)
    {
        Profile6 profile(gen6[i].toObject());

        if (profile == *this)
        {
            gen6.removeAt(i);
            profiles["gen6"] = gen6;

            setting.setValue("profiles", QJsonDocument(profiles).toJson());
            break;
        }
    }
}

void Profile6::updateProfile(const Profile6 &original)
{
    QSettings setting;

    QJsonObject profiles = QJsonDocument::fromJson(setting.value("profiles").toByteArray()).object();
    QJsonArray gen6 = profiles["gen6"].toArray();

    for (auto &&i : gen6)
    {
        Profile6 profile(i.toObject());

        if (original == profile && original != *this)
        {
            i = getJson();
            profiles["gen6"] = gen6;

            setting.setValue("profiles", QJsonDocument(profiles).toJson());
            break;
        }
    }
}

bool operator==(const Profile6 &left, const Profile6 &right)
{
    return left.name == right.name && left.saveVariable == right.saveVariable && left.timeVariable == right.timeVariable
        && left.tid == right.tid && left.sid == right.sid && left.version == right.version
        && left.shinyCharm == right.shinyCharm;
}

bool operator!=(const Profile6 &left, const Profile6 &right)
{
    return !(left == right);
}
