/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2024 by Admiral_Fish
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

#include "Profile.hpp"
#include <Core/Util/Game.hpp>

Profile::Profile() : name("None"), tid(12345), sid(54321), version(Game::UltraSun), shinyCharm(false)
{
}

Profile::Profile(const std::string &name, u16 tid, u16 sid, Game version, bool shinyCharm) :
    name(name), tid(tid), sid(sid), version(version), shinyCharm(shinyCharm)
{
}

std::string Profile::getName() const
{
    return name;
}

u16 Profile::getTID() const
{
    return tid;
}

u16 Profile::getSID() const
{
    return sid;
}

Game Profile::getVersion() const
{
    return version;
}

std::string Profile::getVersionString() const
{
    switch (version)
    {
    case Game::X:
        return "X";
    case Game::Y:
        return "Y";
    case Game::OR:
        return "Omega Ruby";
    case Game::AS:
        return "Alpha Sapphire";
    case Game::Sun:
        return "Sun";
    case Game::Moon:
        return "Moon";
    case Game::UltraSun:
        return "Ultra Sun";
    case Game::UltraMoon:
        return "Ultra Moon";
    default:
        return "-";
    }
}

bool Profile::getShinyCharm() const
{
    return shinyCharm;
}

bool Profile::operator==(const Profile &other) const
{
    return name == other.name && version == other.version && tid == other.tid && sid == other.sid && shinyCharm == other.shinyCharm;
}

bool Profile::operator!=(const Profile &other) const
{
    return !(operator==(other));
}
