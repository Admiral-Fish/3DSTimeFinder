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

#include "ProfileModel7.hpp"
#include <Core/Gen7/Profile7.hpp>

ProfileModel7::ProfileModel7(QObject *parent) : TableModel<Profile7>(parent)
{
}

int ProfileModel7::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return 7;
}

QVariant ProfileModel7::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const auto &profile = model[index.row()];
        switch (index.column())
        {
        case 0:
            return QString::fromStdString(profile.getName());
        case 1:
            return QString::fromStdString(profile.getVersionString());
        case 2:
            return profile.getTID();
        case 3:
            return profile.getSID();
        case 4:
            return profile.getShinyCharm() ? "Yes" : "No";
        case 5:
            return QString::number(profile.getTick(), 16);
        case 6:
            return profile.getOffset();
        }
    }
    return QVariant();
}

QVariant ProfileModel7::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return header[section];
    }
    return QVariant();
}
