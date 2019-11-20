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

#ifndef EVENT6_HPP
#define EVENT6_HPP

#include <Core/Gen6/Profile6.hpp>
#include <Models/EventModel.hpp>
#include <QWidget>

namespace Ui
{
    class Event6;
}

class Event6 : public QWidget
{
    Q_OBJECT
signals:
    void alertProfiles(int);

public:
    explicit Event6(QWidget *parent = nullptr);
    ~Event6() override;
    void updateProfiles();

private:
    Ui::Event6 *ui;
    EventModel *model;
    QVector<Profile6> profiles;

    void setupModels();

private slots:
    void search();
    void update(const QVector<EventResult> &frames, int val);
    void profileManager();
    void profilesIndexChanged(int index);
    void checkBoxEventAbilityLock(bool checked);
    void comboBoxEventPIDTypeIndexChanged(int index);
    void importEvent();
};

#endif // EVENT6_HPP
