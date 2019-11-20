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

#ifndef EVENT7_HPP
#define EVENT7_HPP

#include <Core/Gen7/Profile7.hpp>
#include <Models/EventModel.hpp>
#include <QWidget>

namespace Ui
{
    class Event7;
}

class Event7 : public QWidget
{
    Q_OBJECT
signals:
    void alertProfiles(int);

public:
    explicit Event7(QWidget *parent = nullptr);
    ~Event7() override;
    void updateProfiles();

private:
    Ui::Event7 *ui;
    EventModel *model;
    QVector<Profile7> profiles;

    void setupModel();

private slots:
    void search();
    void update(const QVector<EventResult> &frames, int val);
    void profileManager();
    void profilesIndexChanged(int index);
    void checkBoxEventAbilityLock(bool checked);
    void comboBoxEventPIDTypeIndexChanged(int index);
    void importEvent();
};

#endif // EVENT7_HPP
