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

#ifndef PROFILEEDITOR7_HPP
#define PROFILEEDITOR7_HPP

#include <Core/Gen7/Profile7.hpp>
#include <QDialog>

namespace Ui
{
    class ProfileEditor7;
}

class ProfileEditor7 : public QDialog
{
    Q_OBJECT
signals:
    void newProfile(Profile7);
    void editProfile(Profile7, Profile7);

public:
    explicit ProfileEditor7(QWidget *parent = nullptr);
    explicit ProfileEditor7(const Profile7 &profile, QWidget *parent = nullptr);
    explicit ProfileEditor7(u32 tick, u32 offset, QWidget *parent = nullptr);
    ~ProfileEditor7() override;
    Profile7 getNewProfile();
    Profile7 getOriginal();

private:
    Ui::ProfileEditor7 *ui;
    Profile7 original;
    Profile7 fresh;

    void setupModels();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // PROFILEEDITOR7_HPP
