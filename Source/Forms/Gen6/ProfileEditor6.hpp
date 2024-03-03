/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2024 by Admiral_Fish
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

#ifndef PROFILEEDITOR_HPP
#define PROFILEEDITOR_HPP

#include <Core/Gen6/Profile6.hpp>
#include <QDialog>

namespace Ui
{
    class ProfileEditor6;
}

class ProfileEditor6 : public QDialog
{
    Q_OBJECT
signals:
    void newProfile(Profile6);
    void editProfile(Profile6, Profile6);

public:
    explicit ProfileEditor6(QWidget *parent = nullptr);
    explicit ProfileEditor6(const Profile6 &profile, QWidget *parent = nullptr);
    explicit ProfileEditor6(u32 tick, u32 offset, QWidget *parent = nullptr);
    ~ProfileEditor6() override;
    Profile6 getNewProfile();
    Profile6 getOriginal();

private:
    Ui::ProfileEditor6 *ui;
    Profile6 original;
    Profile6 fresh;

    void setupModels();

private slots:
    void accepted();
};

#endif // PROFILEEDITOR_HPP
