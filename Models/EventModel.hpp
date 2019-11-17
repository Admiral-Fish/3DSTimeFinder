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

#ifndef EVENTMODEL_HPP
#define EVENTMODEL_HPP

#include <Core/Parents/EventResult.hpp>
#include <Models/TableModel.hpp>

class EventModel : public TableModel<EventResult>
{
public:
    EventModel(QObject *parent = nullptr);
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QStringList header = { tr("Date/Time"), tr("Initial Seed"), tr("Frame"), tr("HP"), tr("Atk"), tr("Def"), tr("SpA"),
        tr("SpD"), tr("Spe"), tr("Nature"), tr("HP Type"), tr("PSV"), tr("Gender"), tr("Ability") };
};

#endif // EVENTMODEL_HPP
