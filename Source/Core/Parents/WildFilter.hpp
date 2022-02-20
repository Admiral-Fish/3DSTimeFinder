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

#ifndef WILDFILTER_HPP
#define WILDFILTER_HPP

#include <Core/Parents/ResultFilter.hpp>

class WildResult;

class WildFilter : public ResultFilter
{
public:
    WildFilter(const std::array<u8, 6> &minIV, const std::array<u8, 6> &maxIV, const std::vector<bool> &nature,
               const std::vector<bool> &hiddenPower, const std::vector<bool> &encounterSlots, u8 ability, u8 shiny, u8 gender);
    bool compare(const WildResult &frame);

private:
    std::vector<bool> encounterSlots;
};

#endif // WILDFILTER_HPP
