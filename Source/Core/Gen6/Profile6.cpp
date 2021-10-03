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

#include "Profile6.hpp"

Profile6::Profile6() : saveVariable(0), timeVariable(0)
{
}

Profile6::Profile6(const std::string &name, u32 saveVariable, u32 timeVariable, u16 tid, u16 sid, Game version, bool shinyCharm) :
    Profile(name, tid, sid, version, shinyCharm), saveVariable(saveVariable), timeVariable(timeVariable)
{
}

u32 Profile6::getSaveVariable() const
{
    return saveVariable;
}

u32 Profile6::getTimeVariable() const
{
    return timeVariable;
}

bool operator==(const Profile6 &left, const Profile6 &right)
{
    return left.name == right.name && left.saveVariable == right.saveVariable && left.timeVariable == right.timeVariable
        && left.tid == right.tid && left.sid == right.sid && left.version == right.version && left.shinyCharm == right.shinyCharm;
}

bool operator!=(const Profile6 &left, const Profile6 &right)
{
    return !(left == right);
}
