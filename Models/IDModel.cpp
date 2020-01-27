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

#include "IDModel.hpp"

IDModel::IDModel(QObject *parent)
    : TableModel<IDResult>(parent)
{
}

int IDModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return 7;
}

QVariant IDModel::data(const QModelIndex &index, int role) const
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
            return frame.getDisplayTID();
        case 4:
            return frame.getTID();
        case 5:
            return frame.getSID();
        case 6:
            return frame.getTSV();
        }
    }
    return QVariant();
}

QVariant IDModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return header.at(section);
    }
    return QVariant();
}
