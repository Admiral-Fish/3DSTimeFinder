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

#include "WildModel.hpp"
#include <Core/Parents/WildResult.hpp>
#include <Core/Util/Utility.hpp>

WildModel::WildModel(QObject *parent) : TableModel<WildResult>(parent)
{
}

int WildModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return 17;
}

QVariant WildModel::data(const QModelIndex &index, int role) const
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
            return QString::number(frame.getPID(), 16).toUpper();
        case 4:
            return QString::number(frame.getEC(), 16).toUpper();
        case 5:
        {
            u8 shiny = frame.getShiny();
            return shiny == 2 ? tr("Square") : shiny == 1 ? tr("Star") : tr("No");
        }
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            return frame.getIV(static_cast<u8>(column - 6));
        case 12:
            return QString::fromStdString(*Utility::getNature(frame.getNature()));
        case 13:
            return QString::fromStdString(*Utility::getHiddenPower(frame.getHiddenPower()));
        case 14:
            return QString::fromStdString(frame.getGenderString());
        case 15:
            return QString::fromStdString(frame.getAbilityString());
        case 16:
            return frame.getEncounterSlot();
        }
    }
    return QVariant();
}

QVariant WildModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return header[section];
    }
    return QVariant();
}
