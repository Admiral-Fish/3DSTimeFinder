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

#include "ProfileCalibrater7.hpp"
#include "ui_ProfileCalibrater7.h"
#include <Core/Gen7/ProfileSearcher7.hpp>
#include <Core/Parents/ProfileLoader.hpp>
#include <Core/Util/DateTime.hpp>
#include <Forms/Gen7/ProfileEditor7.hpp>
#include <QMenu>
#include <QSettings>
#include <QStandardItemModel>
#include <QThread>
#include <QTimer>

ProfileCalibrater7::ProfileCalibrater7(QWidget *parent) : QWidget(parent), ui(new Ui::ProfileCalibrater7)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose);

    setupModels();
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
    model->setHorizontalHeaderLabels(QStringList() << "Tick"
                                                   << "Offset");
    ui->tableView->setModel(model);

    ui->textBoxInitialSeed->setValues(InputType::Seed32Bit);
    ui->textBoxBaseTick->setValues(InputType::Seed32Bit);
    ui->textBoxBaseOffset->setValues(InputType::Frame32Bit);
    ui->textBoxTickRange->setValues(InputType::Frame32Bit);
    ui->textBoxOffsetRange->setValues(InputType::Frame32Bit);

    ui->dateTimeEdit->setCalendarPopup(true);
    QDateTime dt(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEdit->setMinimumDateTime(dt);

    contextMenu = new QMenu(ui->tableView);
    QAction *createProfile = contextMenu->addAction("Create profile from parameters");
    connect(createProfile, &QAction::triggered, this, &ProfileCalibrater7::createProfile);

    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &ProfileCalibrater7::search);
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ProfileCalibrater7::indexChanged);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &ProfileCalibrater7::tableViewContextMenu);

    indexChanged(0);

    QSettings setting;
    if (setting.contains("profileCalibrater/geometry"))
    {
        this->restoreGeometry(setting.value("profileCalibrater/geometry").toByteArray());
    }
}

void ProfileCalibrater7::search()
{
    model->removeRows(0, model->rowCount());
    ui->pushButtonSearch->setEnabled(false);
    ui->pushButtonCancel->setEnabled(true);

    DateTime dateTime = ui->dateTimeEdit->getDateTime();
    u32 initialSeed = ui->textBoxInitialSeed->getUInt();
    u32 baseTick = ui->textBoxBaseTick->getUInt();
    u32 baseOffset = ui->textBoxBaseOffset->getUInt();
    u32 tickRange = ui->textBoxTickRange->getUInt();
    u32 offsetRange = ui->textBoxOffsetRange->getUInt();

    auto *searcher = new ProfileSearcher7(dateTime, initialSeed, baseTick, baseOffset, tickRange, offsetRange);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, [=] { searcher->cancelSearch(); });

    ui->progressBar->setRange(0, searcher->getMaxProgress());

    auto *thread = QThread::create([=] { searcher->startSearch(); });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(ui->pushButtonCancel, &QPushButton::clicked, [searcher] { searcher->cancelSearch(); });

    auto *timer = new QTimer();
    connect(timer, &QTimer::timeout, [=] {
        ui->progressBar->setValue(searcher->getProgress());
        auto results = searcher->getResults();
        for (const auto &result : results)
        {
            model->appendRow({ new QStandardItem(QString::number(result.first, 16)), new QStandardItem(QString::number(result.second)) });
        }
    });
    connect(thread, &QThread::finished, timer, &QTimer::stop);
    connect(thread, &QThread::finished, timer, &QTimer::deleteLater);
    connect(timer, &QTimer::destroyed, [=] {
        ui->pushButtonSearch->setEnabled(true);
        ui->pushButtonCancel->setEnabled(false);

        ui->progressBar->setValue(searcher->getProgress());
        auto results = searcher->getResults();
        for (const auto &result : results)
        {
            model->appendRow({ new QStandardItem(QString::number(result.first, 16)), new QStandardItem(QString::number(result.second)) });
        }

        delete searcher;
    });

    thread->start();
    timer->start(1000);
}

void ProfileCalibrater7::indexChanged(int index)
{
    if (index == 0)
    {
        ui->textBoxBaseOffset->setText("55");
        ui->textBoxBaseTick->setText("3730114");
    }
    else if (index == 1)
    {
        ui->textBoxBaseOffset->setText("56");
        ui->textBoxBaseTick->setText("4470937");
    }
}

void ProfileCalibrater7::tableViewContextMenu(QPoint pos)
{
    contextMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void ProfileCalibrater7::createProfile()
{
    u32 tick = ui->tableView->model()
                   ->data(ui->tableView->model()->index(ui->tableView->currentIndex().row(), 0))
                   .toString()
                   .toUInt(nullptr, 16);
    u32 offset = ui->tableView->model()->data(ui->tableView->model()->index(ui->tableView->currentIndex().row(), 1)).toString().toUInt();

    QScopedPointer<ProfileEditor7> dialog(new ProfileEditor7(tick, offset));
    if (dialog->exec() == QDialog::Accepted)
    {
        ProfileLoader7::addProfile(dialog->getNewProfile());
        emit updateProfiles();
    }
}
