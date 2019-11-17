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

#ifndef STATIONARYFILTER_HPP
#define STATIONARYFILTER_HPP

#include <Core/Parents/Filter.hpp>
#include <Core/Parents/StationaryResult.hpp>

class StationaryFilter : public Filter
{
public:
    StationaryFilter() = default;
    StationaryFilter(const QVector<u8> &minIV, const QVector<u8> &maxIV, const QVector<bool> &nature,
        const QVector<bool> &hiddenPower, u8 ability, bool shiny, u8 gender);
    bool compare(const StationaryResult &frame);
};

#endif // STATIONARYFILTER_HPP
