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

#ifndef PROFILE7_HPP
#define PROFILE7_HPP

#include <Core/Parents/Profile.hpp>
#include <QJsonObject>

class Profile7 : public Profile
{
public:
    Profile7();
    Profile7(const QString &name, u32 offset, u32 tick, u16 tid, u16 sid, Game version, bool shinyCharm);
    explicit Profile7(QJsonObject data);
    QJsonObject getJson();
    u32 getOffset() const;
    u32 getTick() const;
    static std::vector<Profile7> loadProfileList();
    void saveProfile();
    void deleteProfile();
    void updateProfile(const Profile7 &original);
    friend bool operator==(const Profile7 &left, const Profile7 &right);
    friend bool operator!=(const Profile7 &left, const Profile7 &right);

private:
    u32 offset;
    u32 tick;
};
bool operator==(const Profile7 &left, const Profile7 &right);
bool operator!=(const Profile7 &left, const Profile7 &right);

#endif // PROFILE7_HPP
