/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2021 by Admiral_Fish
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

#ifndef PROFILELOADER_HPP
#define PROFILELOADER_HPP

#include <Core/Gen6/Profile6.hpp>
#include <Core/Gen7/Profile7.hpp>
#include <vector>

namespace ProfileLoader
{
    bool init(const std::string &location);
}

namespace ProfileLoader6
{
    std::vector<Profile6> getProfiles();
    void addProfile(const Profile6 &profile);
    void removeProfile(const Profile6 &remove);
    void updateProfile(const Profile6 &update, const Profile6 &original);
}

namespace ProfileLoader7
{
    std::vector<Profile7> getProfiles();
    void addProfile(const Profile7 &profile);
    void removeProfile(const Profile7 &remove);
    void updateProfile(const Profile7 &update, const Profile7 &original);
}

#endif // PROFILELOADER_HPP
