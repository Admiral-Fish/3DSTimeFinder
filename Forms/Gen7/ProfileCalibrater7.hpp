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

#ifndef PROFILECALIBRATER7_HPP
#define PROFILECALIBRATER7_HPP

#include <Core/Global.hpp>
#include <QMenu>
#include <QStandardItemModel>
#include <QVector>

namespace Ui
{
    class ProfileCalibrater7;
}

class ProfileCalibrater7 : public QWidget
{
    Q_OBJECT
signals:
    void updateProfiles();

public:
    explicit ProfileCalibrater7(QWidget *parent = nullptr);
    ~ProfileCalibrater7() override;

private:
    Ui::ProfileCalibrater7 *ui;
    QStandardItemModel *model {};
    QMenu *contextMenu {};

    void setupModels();

private slots:
    void on_pushButtonSearch_clicked();
    void updateResults(QVector<QPair<u32, u32>> results, int val);
    void on_comboBox_currentIndexChanged(int index);
    void on_tableView_customContextMenuRequested(const QPoint &pos);
    void createProfile();
};

#endif // PROFILECALIBRATER7_HPP
