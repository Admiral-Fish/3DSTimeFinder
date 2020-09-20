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
                tidFilter.append(in.toUShort());
                break;
            case IDType::SID:
                sidFilter.append(in.toUShort());
                break;
            case IDType::TIDSID:
            {
                QStringList split = in.split("/");
                tidFilter.append(split.at(0).toUShort());
                sidFilter.append(split.at(1).toUShort());
                break;
            }
            case IDType::G7TID:
                g7Filter.append(in.toUInt());
                break;
            }
        }
    }

    if (!tsvList.isEmpty())
    {
        for (const QString &in : tsvList.split("\n"))
        {
            tsvFilter.append(in.toUInt());
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
            if (!tidFilter.contains(frame.getTID()))
            {
                return false;
            }
            break;
        case IDType::SID:
            if (!sidFilter.contains(frame.getSID()))
            {
                return false;
            }
            break;
        case IDType::TIDSID:
            if (!tidFilter.contains(frame.getTID()) || !sidFilter.contains(frame.getSID()))
            {
                return false;
            }
            break;
        case IDType::G7TID:
            if (!g7Filter.contains(frame.getDisplayTID()))
            {
                return false;
            }
            break;
        }
    }

    if (checkTSV)
    {
        if (!tsvFilter.contains(frame.getTSV()))
        {
            return false;
        }
    }

    return true;
}
