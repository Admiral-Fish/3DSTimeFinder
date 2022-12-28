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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class Event6;
class Stationary6;
class Event7;
class ID7;
class QActionGroup;
class Stationary7;
class Wild7;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(bool profile, QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QActionGroup *styleGroup;
    QActionGroup *threadGroup;
    QString currentStyle;
    Event6 *event6 = nullptr;
    Stationary6 *stationary6 = nullptr;
    Event7 *event7 = nullptr;
    ID7 *id7 = nullptr;
    Stationary7 *stationary7 = nullptr;
    Wild7 *wild7 = nullptr;

    void setupModel();
    void setupStyle();
    void setupThread();

private slots:
    void slotStyleChanged(QAction *action);
    void slotThreadChanged(QAction *action);
    void updateProfilePath();
    void updateProfiles(int num);
    void openEvent6();
    void openStationary6();
    void openEvent7();
    void openID7();
    void openStationary7();
    void openWild7();
    void openAbout();
    void openCalibrator7();
};
#endif // MAINWINDOW_HPP
