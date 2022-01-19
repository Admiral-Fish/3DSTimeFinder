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

#ifndef IDFILTER_HPP
#define IDFILTER_HPP

#include <Core/Parents/IDResult.hpp>
#include <Core/Util/IDType.hpp>
#include <vector>

class IDFilter
{
public:
    IDFilter() = default;
    IDFilter(const std::string &idList, const std::string &tsvList, IDType type);
    bool compare(const IDResult &frame);

private:
    std::vector<u16> tidFilter;
    std::vector<u16> sidFilter;
    std::vector<u16> tsvFilter;
    std::vector<u32> g7Filter;
    IDType idType;
    bool checkID;
    bool checkTSV;
};

#endif // IDFILTER_HPP
