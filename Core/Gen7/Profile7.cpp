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

#include "Profile7.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>
#include <QVector>

Profile7::Profile7() : Profile(), offset(55), tick(0x41D9CB9)
{
    version = Game::UltraSun;
}

Profile7::Profile7(const QString &name, u32 offset, u32 tick, u16 tid, u16 sid, Game version, bool shinyCharm)
    : Profile(name, tid, sid, version, shinyCharm)
{
    this->offset = offset;
    this->tick = tick;
}

Profile7::Profile7(QJsonObject data) :
    Profile(data["name"].toString(), data["tid"].toInt(), data["sid"].toInt(), static_cast<Game>(data["version"].toInt()),
            data["charm"].toBool()),
    offset(data["offset"].toString().toUInt()),
    tick(data["tick"].toString().toUInt(nullptr, 16))
{
}

QJsonObject Profile7::getJson()
{
    QJsonObject data;
    data["name"] = name;
    data["offset"] = QString::number(offset);
    data["tick"] = QString::number(tick, 16);
    data["tid"] = tid;
    data["sid"] = sid;
    data["version"] = version;
    data["charm"] = shinyCharm;
    return data;
}

u32 Profile7::getOffset() const
{
    return offset;
}

u32 Profile7::getTick() const
{
    return tick;
}

std::vector<Profile7> Profile7::loadProfileList()
{
    std::vector<Profile7> profileList;
    QSettings setting;

    QJsonObject profiles(QJsonDocument::fromJson(setting.value("profiles").toByteArray()).object());
    QJsonArray gen7 = profiles["gen7"].toArray();

    for (const auto &&i : gen7)
    {
        auto data = i.toObject();
        profileList.emplace_back(Profile7(data));
    }

    return profileList;
}

void Profile7::saveProfile()
{
    QSettings setting;

    QJsonObject profiles(QJsonDocument::fromJson(setting.value("profiles").toByteArray()).object());
    QJsonArray gen7 = profiles["gen7"].toArray();

    gen7.append(getJson());
    profiles["gen7"] = gen7;

    setting.setValue("profiles", QJsonDocument(profiles).toJson());
}

void Profile7::deleteProfile()
{
    QSettings setting;

    QJsonObject profiles(QJsonDocument::fromJson(setting.value("profiles").toByteArray()).object());
    QJsonArray gen7 = profiles["gen7"].toArray();

    for (u8 i = 0; i < gen7.size(); i++)
    {
        Profile7 profile(gen7[i].toObject());

        if (profile == *this)
        {
            gen7.removeAt(i);
            profiles["gen7"] = gen7;

            setting.setValue("profiles", QJsonDocument(profiles).toJson());
            break;
        }
    }
}

void Profile7::updateProfile(const Profile7 &original)
{
    QSettings setting;

    QJsonObject profiles = QJsonDocument::fromJson(setting.value("profiles").toByteArray()).object();
    QJsonArray gen7 = profiles["gen7"].toArray();

    for (auto &&i : gen7)
    {
        Profile7 profile(i.toObject());

        if (original == profile && original != *this)
        {
            i = getJson();
            profiles["gen7"] = gen7;

            setting.setValue("profiles", QJsonDocument(profiles).toJson());
            break;
        }
    }
}

bool operator==(const Profile7 &left, const Profile7 &right)
{
    return left.name == right.name && left.offset == right.offset && left.tick == right.tick && left.tid == right.tid
        && left.sid == right.sid && left.version == right.version && left.shinyCharm == right.shinyCharm;
}

bool operator!=(const Profile7 &left, const Profile7 &right)
{
    return !(left == right);
}
