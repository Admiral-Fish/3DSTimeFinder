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

#include "EventModel.hpp"
#include <Core/Util/Utility.hpp>

EventModel::EventModel(QObject *parent) : TableModel<EventResult>(parent)
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
        const auto &frame = model[index.row()];
        int column = index.column();
        switch (column)
        {
        case 0:
            return QString::fromStdString(frame.getDateTime());
        case 1:
            return QString::number(frame.getSeed(), 16).toUpper();
        case 2:
            return frame.getFrame();
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            return frame.getIV(static_cast<u8>(column - 3));
        case 9:
            return QString::fromStdString(Utility::getNature(frame.getNature()));
        case 10:
            return QString::fromStdString(Utility::getHiddenPower(frame.getHiddenPower()));
        case 11:
            return frame.getPSV();
        case 12:
            return QString::fromStdString(frame.getGenderString());
        case 13:
            return QString::fromStdString(frame.getAbilityString());
        }
    }
    return QVariant();
}

QVariant EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return header[section];
    }
    return QVariant();
}
