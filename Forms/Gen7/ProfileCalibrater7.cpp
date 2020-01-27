/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2020 by Admiral_Fish
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
#include <QTimer>
#include <QtConcurrent>

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
    QDateTime dt(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEdit->setMinimumDateTime(dt);

    contextMenu = new QMenu(ui->tableView);
    QAction *createProfile = contextMenu->addAction(tr("Create profile from parameters"));
    connect(createProfile, &QAction::triggered, this, &ProfileCalibrater7::createProfile);

    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &ProfileCalibrater7::search);
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ProfileCalibrater7::indexChanged);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &ProfileCalibrater7::tableViewContextMenu);

    QSettings setting;
    if (setting.contains("profileCalibrater/geometry"))
        this->restoreGeometry(setting.value("profileCalibrater/geometry").toByteArray());
}

void ProfileCalibrater7::search()
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

    auto *searcher = new ProfileSearcher7(dateTime, initialSeed, baseTick, baseOffset, tickRange, offsetRange);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, [=] { searcher->cancelSearch(); });

    ui->progressBar->setRange(0, searcher->getMaxProgress());

    auto *timer = new QTimer();
    connect(timer, &QTimer::timeout, [=] {
        ui->progressBar->setValue(searcher->getProgress());
        auto results = searcher->getResults();
        for (const auto &result : results)
        {
            model->appendRow({ new QStandardItem(QString::number(result.first, 16)), new QStandardItem(QString::number(result.second)) });
        }
    });

    auto *watcher = new QFutureWatcher<void>();
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);
    connect(watcher, &QFutureWatcher<void>::destroyed, this, [=] {
        ui->pushButtonSearch->setEnabled(true);
        ui->pushButtonCancel->setEnabled(false);

        timer->stop();
        delete timer;

        ui->progressBar->setValue(searcher->getProgress());
        auto results = searcher->getResults();
        for (const auto &result : results)
        {
            model->appendRow({ new QStandardItem(QString::number(result.first, 16)), new QStandardItem(QString::number(result.second)) });
        }

        delete searcher;
    });

    auto future = QtConcurrent::run([=] { searcher->startSearch(); });

    watcher->setFuture(future);
    timer->start(1000);
}

void ProfileCalibrater7::indexChanged(int index)
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
    u32 offset = ui->tableView->model()
                     ->data(ui->tableView->model()->index(ui->tableView->currentIndex().row(), 1))
                     .toString()
                     .toUInt();

    QScopedPointer<ProfileEditor7> dialog(new ProfileEditor7(tick, offset));
    if (dialog->exec() == QDialog::Accepted)
    {
        Profile7 profile = dialog->getNewProfile();
        profile.saveProfile();
        emit updateProfiles();
    }
}
