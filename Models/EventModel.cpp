/*
 * This file is part of Gen7TimeFinder
 * Copyright (C) 2018-2019 by Admiral_Fish
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

#include "EventModel.hpp"
#include <Core/Util/Utility.hpp>

EventModel::EventModel(QObject *parent)
    : TableModel<EventResult>(parent)
{
}

int EventModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return 14;
}

QVariant EventModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        auto &frame = model.at(index.row());
        switch (index.column())
        {
        case 0:
            return frame.getDateTime();
        case 1:
            return QString::number(frame.getSeed(), 16).toUpper();
        case 2:
            return frame.getFrame();
        case 3:
            return frame.getIV(0);
        case 4:
            return frame.getIV(1);
        case 5:
            return frame.getIV(2);
        case 6:
            return frame.getIV(3);
        case 7:
            return frame.getIV(4);
        case 8:
            return frame.getIV(5);
        case 9:
            return Utility::getNature(frame.getNature());
        case 10:
            return Utility::getHiddenPower(frame.getHiddenPower());
        case 11:
            return frame.getPSV();
        case 12:
            return frame.getGenderString();
        case 13:
            return frame.getAbilityString();
        }
    }
    return QVariant();
}

QVariant EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return header.at(section);
    }
    return QVariant();
}
