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

#ifndef STATIONARY7_HPP
#define STATIONARY7_HPP

#include <QWidget>

class Profile7;
class StationaryModel;

namespace Ui
{
    class Stationary7;
}

class Stationary7 : public QWidget
{
    Q_OBJECT
signals:
    void alertProfiles(int);

public:
    explicit Stationary7(QWidget *parent = nullptr);
    ~Stationary7() override;
    void updateProfiles();

private:
    Ui::Stationary7 *ui;
    StationaryModel *model;
    std::vector<Profile7> profiles;

    void setupModel();

private slots:
    void search();
    void profileManager();
    void profilesIndexChanged(int index);
};

#endif // STATIONARY7_HPP
