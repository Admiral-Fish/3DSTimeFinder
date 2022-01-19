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

#include "Profile7.hpp"

Profile7::Profile7() : offset(55), tick(0x41D9CB9)
{
    version = Game::UltraSun;
}

Profile7::Profile7(const std::string &name, u32 offset, u32 tick, u16 tid, u16 sid, Game version, bool shinyCharm) :
    Profile(name, tid, sid, version, shinyCharm)
{
    this->offset = offset;
    this->tick = tick;
}

u32 Profile7::getOffset() const
{
    return offset;
}

u32 Profile7::getTick() const
{
    return tick;
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
