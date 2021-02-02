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

#include "IDFilter.hpp"

IDFilter::IDFilter(const QString &idList, const QString &tsvList, IDType type) : idType(type)
{
    checkID = !idList.isEmpty();
    checkTSV = !tsvList.isEmpty();

    if (!idList.isEmpty())
    {
        for (const QString &in : idList.split("\n"))
        {
            switch (idType)
            {
            case IDType::TID:
                tidFilter.emplace_back(in.toUShort());
                break;
            case IDType::SID:
                sidFilter.emplace_back(in.toUShort());
                break;
            case IDType::TIDSID:
            {
                QStringList split = in.split("/");
                tidFilter.emplace_back(split.at(0).toUShort());
                sidFilter.emplace_back(split.at(1).toUShort());
                break;
            }
            case IDType::G7TID:
                g7Filter.emplace_back(in.toUInt());
                break;
            }
        }
    }

    if (!tsvList.isEmpty())
    {
        for (const QString &in : tsvList.split("\n"))
        {
            tsvFilter.emplace_back(in.toUInt());
        }
    }
}

bool IDFilter::compare(const IDResult &frame)
{
    if (checkID)
    {
        switch (idType)
        {
        case IDType::TID:
            if (std::find(tidFilter.begin(), tidFilter.end(), frame.getTID()) == tidFilter.end())
            {
                return false;
            }
            break;
        case IDType::SID:
            if (std::find(sidFilter.begin(), sidFilter.end(), frame.getSID()) == sidFilter.end())
            {
                return false;
            }
            break;
        case IDType::TIDSID:
            if (std::find(tidFilter.begin(), tidFilter.end(), frame.getTID()) == tidFilter.end()
                || std::find(sidFilter.begin(), sidFilter.end(), frame.getSID()) == sidFilter.end())
            {
                return false;
            }
            break;
        case IDType::G7TID:
            if (std::find(g7Filter.begin(), g7Filter.end(), frame.getDisplayTID()) == g7Filter.end())
            {
                return false;
            }
            break;
        }
    }

    if (checkTSV)
    {
        if (std::find(tsvFilter.begin(), tsvFilter.end(), frame.getTSV()) == tsvFilter.end())
        {
            return false;
        }
    }

    return true;
}
