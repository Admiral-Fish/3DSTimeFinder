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

#include "Stationary7.hpp"
#include "ui_Stationary7.h"
#include <Core/Gen7/StationarySearcher7.hpp>
#include <Core/Util/Utility.hpp>
#include <Forms/Gen7/ProfileManager7.hpp>
#include <QMessageBox>
#include <QSettings>

Stationary7::Stationary7(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Stationary7)
{
    ui->setupUi(this);

    setupModel();
    updateProfiles();

    qRegisterMetaType<QVector<StationaryResult>>("QVector<StationaryResult");
}

Stationary7::~Stationary7()
{
    QSettings settings;
    settings.setValue("stationary7/geometry", this->saveGeometry());

    delete ui;
}

void Stationary7::updateProfiles()
{
    profiles = Profile7::loadProfileList();
    profiles.insert(profiles.begin(), Profile7());

    QSettings setting;
    int val = setting.value("stationary7/profile", 0).toInt();

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

void Stationary7::setupModel()
{
    model = new StationaryModel(ui->tableViewStationary);

    ui->tableViewStationary->setModel(model);

    auto natures = Utility::getNatures();
    auto hiddenPowers = Utility::getHiddenPowers();
    auto genderRatios = Utility::getGenderRatios();

    ui->comboBoxStationaryNature->addItems(natures);
    ui->comboBoxStationaryNature->setup();
    ui->comboBoxStationaryHiddenPower->addItems(hiddenPowers);
    ui->comboBoxStationaryHiddenPower->setup();
    ui->comboBoxStationarySynchNature->addItems(natures);
    ui->comboBoxStationaryGenderRatio->addItems(genderRatios);

    ui->comboBoxStationaryGenderRatio->setItemData(0, 0);
    ui->comboBoxStationaryGenderRatio->setItemData(1, 126);
    ui->comboBoxStationaryGenderRatio->setItemData(2, 30);
    ui->comboBoxStationaryGenderRatio->setItemData(3, 62);
    ui->comboBoxStationaryGenderRatio->setItemData(4, 190);
    ui->comboBoxStationaryGenderRatio->setItemData(5, 224);
    ui->comboBoxStationaryGenderRatio->setItemData(6, 1);
    ui->comboBoxStationaryGenderRatio->setItemData(7, 2);

    ui->textBoxStationaryStartFrame->setValues(InputType::Frame32Bit);
    ui->textBoxStationaryEndFrame->setValues(InputType::Frame32Bit);

    ui->dateTimeEditStationaryStartDate->setCalendarPopup(true);
    ui->dateTimeEditStationaryEndDate->setCalendarPopup(true);

    QDateTime dt(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEditStationaryStartDate->setMinimumDateTime(dt);
    ui->dateTimeEditStationaryEndDate->setMinimumDateTime(dt);

    QSettings setting;
    if (setting.contains("stationary7/geometry"))
        this->restoreGeometry(setting.value("stationary7/geometry").toByteArray());
}

void Stationary7::search()
{
    QDateTime start = ui->dateTimeEditStationaryStartDate->dateTime();
    start.setTimeSpec(Qt::UTC);
    QDateTime end = ui->dateTimeEditStationaryEndDate->dateTime();
    end.setTimeSpec(Qt::UTC);
    u32 frameStart = ui->textBoxStationaryStartFrame->getUInt();
    u32 frameEnd = ui->textBoxStationaryEndFrame->getUInt();

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
    ui->pushButtonStationarySearch->setEnabled(false);
    ui->pushButtonStationaryCancel->setEnabled(true);

    QVector<u8> min = ui->ivFilterStationary->getLower();
    QVector<u8> max = ui->ivFilterStationary->getUpper();

    StationaryFilter filter(min, max, ui->comboBoxStationaryNature->getChecked(),
        ui->comboBoxStationaryHiddenPower->getChecked(), ui->comboBoxStationaryAbility->currentIndex() - 1,
        ui->checkBoxStationaryShiny->isChecked(), ui->comboBoxStationaryGender->currentIndex());

    auto *search = new StationarySearcher7(start, end, frameStart, frameEnd, ui->checkBoxStationary3IVs->isChecked(),
        ui->checkBoxStationaryAbilityLock->isChecked() ? ui->comboBoxStationaryAbilityLock->currentIndex() : -1,
        ui->comboBoxStationarySynchNature->currentIndex(), ui->comboBoxStationaryGenderRatio->currentData().toInt(),
        ui->checkBoxStationaryAlwaysSynch->isChecked(), ui->checkBoxStationaryShinyLock->isChecked(),
        profiles.at(ui->comboBoxProfiles->currentIndex()), filter);

    connect(search, &StationarySearcher7::finished, this, [=] {
        ui->pushButtonStationarySearch->setEnabled(true);
        ui->pushButtonStationaryCancel->setEnabled(false);
    });
    connect(search, &StationarySearcher7::updateProgress, this, &Stationary7::update);
    connect(ui->pushButtonStationaryCancel, &QPushButton::clicked, search, &StationarySearcher7::cancelSearch);

    ui->progressBarStationary->setValue(0);
    ui->progressBarStationary->setMaximum(search->maxProgress());

    search->startSearch();
}

void Stationary7::update(const QVector<StationaryResult> &frames, int val)
{
    model->addItems(frames);
    ui->progressBarStationary->setValue(val);
}

void Stationary7::profileManager()
{
    auto *manager = new ProfileManager7();
    connect(manager, &ProfileManager7::updateProfiles, [=]() { emit alertProfiles(7); });
    manager->show();
    manager->raise();
}

void Stationary7::profilesIndexChanged(int index)
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
        setting.setValue("stationary7/profile", index);
    }
}
