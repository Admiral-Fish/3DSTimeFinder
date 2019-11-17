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

#include "ProfileCalibrater7.hpp"
#include "ui_ProfileCalibrater7.h"
#include <Core/Gen7/ProfileSearcher7.hpp>
#include <Forms/Gen7/ProfileEditor7.hpp>
#include <QSettings>

ProfileCalibrater7::ProfileCalibrater7(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProfileCalibrater7)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose);

    setupModels();

    qRegisterMetaType<QVector<QPair<u32, u32>>>("QVector<QPair<u32, u32>>");
}

ProfileCalibrater7::~ProfileCalibrater7()
{
    QSettings setting;
    setting.setValue("profileCalibrater/geometry", this->saveGeometry());

    delete ui;
}

void ProfileCalibrater7::setupModels()
{
    model = new QStandardItemModel(ui->tableView);
    model->setHorizontalHeaderLabels(QStringList() << tr("Tick") << tr("Offset"));
    ui->tableView->setModel(model);

    ui->textBoxInitialSeed->setValues(InputType::Seed32Bit);
    ui->textBoxBaseTick->setValues(InputType::Seed32Bit);
    ui->textBoxBaseOffset->setValues(InputType::Frame32Bit);
    ui->textBoxTickRange->setValues(InputType::Frame32Bit);
    ui->textBoxOffsetRange->setValues(InputType::Frame32Bit);

    ui->dateTimeEdit->setCalendarPopup(true);
    QDateTime dt;
    dt.fromString(QStringLiteral("2000-01-01 00:00:01"), QStringLiteral("yyyy-MM-dd hh:mm:ss"));
    ui->dateTimeEdit->setMinimumDateTime(dt);

    contextMenu = new QMenu(ui->tableView);
    QAction *createProfile = contextMenu->addAction(tr("Create profile from parameters"));
    connect(createProfile, &QAction::triggered, this, &ProfileCalibrater7::createProfile);

    QSettings setting;
    if (setting.contains("profileCalibrater/geometry"))
        this->restoreGeometry(setting.value("profileCalibrater/geometry").toByteArray());
}

void ProfileCalibrater7::on_pushButtonSearch_clicked()
{
    model->removeRows(0, model->rowCount());
    ui->pushButtonSearch->setEnabled(false);
    ui->pushButtonCancel->setEnabled(true);

    QDateTime dateTime = ui->dateTimeEdit->dateTime();
    dateTime.setTimeSpec(Qt::UTC);
    u32 initialSeed = ui->textBoxInitialSeed->getUInt();
    u32 baseTick = ui->textBoxBaseTick->getUInt();
    u32 baseOffset = ui->textBoxBaseOffset->getUInt();
    u32 tickRange = ui->textBoxTickRange->getUInt();
    u32 offsetRange = ui->textBoxOffsetRange->getUInt();

    auto *search = new ProfileSearcher7(dateTime, initialSeed, baseTick, baseOffset, tickRange, offsetRange);

    connect(search, &ProfileSearcher7::finished, this, [=] {
        ui->pushButtonSearch->setEnabled(true);
        ui->pushButtonCancel->setEnabled(false);
    });
    connect(search, &ProfileSearcher7::updateProgress, this, &ProfileCalibrater7::updateResults);
    connect(ui->pushButtonCancel, &QPushButton::clicked, search, &ProfileSearcher7::cancelSearch);

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(search->maxProgress());

    search->startSearch();
}

void ProfileCalibrater7::updateResults(QVector<QPair<u32, u32>> results, int val)
{
    for (const auto &result : results)
    {
        QList<QStandardItem *> list = { new QStandardItem(QString::number(result.first, 16)),
            new QStandardItem(QString::number(result.second)) };
        model->appendRow(list);
    }

    ui->progressBar->setValue(val);
}

void ProfileCalibrater7::on_comboBox_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->textBoxBaseOffset->setText("54");
        ui->textBoxBaseTick->setText("3532EA4");
    }
    else if (index == 1)
    {
        ui->textBoxBaseOffset->setText("55");
        ui->textBoxBaseTick->setText("41D9CB9");
    }
}

void ProfileCalibrater7::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    contextMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void ProfileCalibrater7::createProfile()
{
    u32 tick = ui->tableView->model()
                   ->data(ui->tableView->model()->index(ui->tableView->currentIndex().row(), 0))
                   .toString()
                   .toUInt(nullptr, 16);
    u32 offset = ui->tableView->model()
                     ->data(ui->tableView->model()->index(ui->tableView->currentIndex().row(), 1))
                     .toString()
                     .toUInt();

    auto *dialog = new ProfileEditor7(tick, offset);
    if (dialog->exec() == QDialog::Accepted)
    {
        Profile7 profile = dialog->getNewProfile();
        profile.saveProfile();
        emit updateProfiles();
    }
}
