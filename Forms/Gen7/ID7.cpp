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

#include "ID7.hpp"
#include "ui_ID7.h"
#include <Core/Gen7/IDSearcher7.hpp>
#include <Forms/Gen7/ProfileManager7.hpp>
#include <Models/IDModel.hpp>
#include <QMessageBox>
#include <QSettings>

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
    model = new IDModel(ui->tableViewID);

    ui->tableViewID->setModel(model);

    ui->textBoxIDStartFrame->setValues(InputType::Frame32Bit);
    ui->textBoxIDEndFrame->setValues(InputType::Frame32Bit);

    ui->dateTimeEditIDStartDate->setCalendarPopup(true);
    ui->dateTimeEditIDEndDate->setCalendarPopup(true);

    QDateTime dt(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEditIDStartDate->setMinimumDateTime(dt);
    ui->dateTimeEditIDEndDate->setMinimumDateTime(dt);

    connect(ui->pushButtonIDSearch, &QPushButton::clicked, this, &ID7::search);
    connect(ui->pushButtonProfileManager, &QPushButton::clicked, this, &ID7::profileManager);
    connect(
        ui->comboBoxProfiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ID7::profilesIndexChanged);

    QSettings setting;
    if (setting.contains("id7/geometry"))
        this->restoreGeometry(setting.value("id7/geometry").toByteArray());
}

void ID7::search()
{
    QDateTime start = ui->dateTimeEditIDStartDate->dateTime();
    start.setTimeSpec(Qt::UTC);
    QDateTime end = ui->dateTimeEditIDEndDate->dateTime();
    end.setTimeSpec(Qt::UTC);
    u32 frameStart = ui->textBoxIDStartFrame->getUInt();
    u32 frameEnd = ui->textBoxIDEndFrame->getUInt();

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
    ui->pushButtonIDSearch->setEnabled(false);
    ui->pushButtonIDCancel->setEnabled(true);

    IDType type;
    if (ui->radioButtonIDTID->isChecked())
    {
        type = IDType::TID;
    }
    else if (ui->radioButtonIDSID->isChecked())
    {
        type = IDType::SID;
    }
    else if (ui->radioButtonIDTIDSID->isChecked())
    {
        type = IDType::TIDSID;
    }
    else
    {
        type = IDType::G7TID;
    }

    IDFilter filter(ui->textEditIDFilter->toPlainText(), ui->textEditTSVFilter->toPlainText(), type);

    auto *search
        = new IDSearcher7(start, end, frameStart, frameEnd, profiles.at(ui->comboBoxProfiles->currentIndex()), filter);

    connect(search, &IDSearcher7::finished, this, [=] {
        ui->pushButtonIDSearch->setEnabled(true);
        ui->pushButtonIDCancel->setEnabled(false);
    });
    connect(search, &IDSearcher7::updateProgress, this, &ID7::update);
    connect(ui->pushButtonIDCancel, &QPushButton::clicked, search, &IDSearcher7::cancelSearch);

    ui->progressBarID->setValue(0);
    ui->progressBarID->setMaximum(search->maxProgress());

    search->startSearch();
}

void ID7::update(const QVector<IDResult> &frames, int val)
{
    model->addItems(frames);
    ui->progressBarID->setValue(val);
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
