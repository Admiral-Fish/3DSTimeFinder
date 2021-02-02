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

#include "ID7.hpp"
#include "ui_ID7.h"
#include <Core/Gen7/IDSearcher7.hpp>
#include <Forms/Gen7/ProfileManager7.hpp>
#include <Models/IDModel.hpp>
#include <QMessageBox>
#include <QSettings>
#include <QThread>
#include <QTimer>

ID7::ID7(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ID7)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModel();
    updateProfiles();

    qRegisterMetaType<QVector<IDResult>>("QVector<IDResult>");
}

ID7::~ID7()
{
    QSettings settings;
    settings.setValue("id7/geometry", this->saveGeometry());

    delete ui;
}

void ID7::updateProfiles()
{
    profiles = Profile7::loadProfileList();
    profiles.insert(profiles.begin(), Profile7());

    QSettings setting;
    int val = setting.value("id7/profile", 0).toInt();

    ui->comboBoxProfiles->clear();

    for (const auto &profile : profiles)
    {
        ui->comboBoxProfiles->addItem(profile.getName());
    }

    if (val < ui->comboBoxProfiles->count())
    {
        ui->comboBoxProfiles->setCurrentIndex(val);
    }
}

void ID7::setupModel()
{
    model = new IDModel(ui->tableView);

    ui->tableView->setModel(model);

    ui->textBoxStartFrame->setValues(InputType::Frame32Bit);
    ui->textBoxEndFrame->setValues(InputType::Frame32Bit);

    ui->dateTimeEditStartDate->setCalendarPopup(true);
    ui->dateTimeEditEndDate->setCalendarPopup(true);

    QDateTime dt(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEditStartDate->setMinimumDateTime(dt);
    ui->dateTimeEditEndDate->setMinimumDateTime(dt);

    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &ID7::search);
    connect(ui->pushButtonProfileManager, &QPushButton::clicked, this, &ID7::profileManager);
    connect(
        ui->comboBoxProfiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ID7::profilesIndexChanged);

    QSettings setting;
    if (setting.contains("id7/geometry"))
    {
        this->restoreGeometry(setting.value("id7/geometry").toByteArray());
    }
}

void ID7::search()
{
    QDateTime start = ui->dateTimeEditStartDate->dateTime();
    start.setTimeSpec(Qt::UTC);
    QDateTime end = ui->dateTimeEditEndDate->dateTime();
    end.setTimeSpec(Qt::UTC);
    u32 frameStart = ui->textBoxStartFrame->getUInt();
    u32 frameEnd = ui->textBoxEndFrame->getUInt();

    if (start > end)
    {
        QMessageBox error;
        error.setText(tr("Set end date to be after start date."));
        error.exec();
        return;
    }
    if (frameStart > frameEnd)
    {
        QMessageBox error;
        error.setText(tr("Set end frame to be after start frame"));
        error.exec();
        return;
    }

    model->clearModel();
    ui->pushButtonSearch->setEnabled(false);
    ui->pushButtonCancel->setEnabled(true);

    IDType type;
    if (ui->radioButtonTID->isChecked())
    {
        type = IDType::TID;
    }
    else if (ui->radioButtonSID->isChecked())
    {
        type = IDType::SID;
    }
    else if (ui->radioButtonTIDSID->isChecked())
    {
        type = IDType::TIDSID;
    }
    else
    {
        type = IDType::G7TID;
    }

    IDFilter filter(ui->textEditFilter->toPlainText(), ui->textEditTSVFilter->toPlainText(), type);

    auto *searcher = new IDSearcher7(start, end, frameStart, frameEnd, profiles.at(ui->comboBoxProfiles->currentIndex()), filter);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, [=] { searcher->cancelSearch(); });

    ui->progressBar->setRange(0, searcher->getMaxProgress());

    QSettings settings;
    int threads = settings.value("settings/threads", QThread::idealThreadCount()).toInt();

    auto *thread = QThread::create([=] { searcher->startSearch(threads); });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(ui->pushButtonCancel, &QPushButton::clicked, [searcher] { searcher->cancelSearch(); });

    auto *timer = new QTimer();
    connect(timer, &QTimer::timeout, [=] {
        ui->progressBar->setValue(searcher->getProgress());
        model->addItems(searcher->getResults());
    });
    connect(thread, &QThread::finished, timer, &QTimer::stop);
    connect(thread, &QThread::finished, timer, &QTimer::deleteLater);
    connect(timer, &QTimer::destroyed, [=] {
        ui->pushButtonSearch->setEnabled(true);
        ui->pushButtonCancel->setEnabled(false);
        ui->progressBar->setValue(searcher->getProgress());
        model->addItems(searcher->getResults());
        delete searcher;
    });

    thread->start();
    timer->start(1000);
}

void ID7::profileManager()
{
    auto *manager = new ProfileManager7();
    connect(manager, &ProfileManager7::updateProfiles, [=]() { emit alertProfiles(7); });
    manager->show();
    manager->raise();
}

void ID7::profilesIndexChanged(int index)
{
    if (index >= 0)
    {
        auto profile = profiles.at(index);
        ui->labelProfileOffsetValue->setText(QString::number(profile.getOffset()));
        ui->labelProfileTickValue->setText(QString::number(profile.getTick(), 16));
        ui->labelProfileTIDValue->setText(QString::number(profile.getTID()));
        ui->labelProfileSIDValue->setText(QString::number(profile.getSID()));
        ui->labelProfileGameValue->setText(profile.getVersionString());
        ui->labelProfileShinyCharmValue->setText(profile.getShinyCharm() ? "Yes" : "No");

        QSettings setting;
        setting.setValue("id7/profile", index);
    }
}
