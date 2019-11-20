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

#include "Wild7.hpp"
#include "ui_Wild7.h"
#include <Core/Gen7/WildSearcher7.hpp>
#include <Forms/Gen7/ProfileManager7.hpp>
#include <QMessageBox>
#include <QSettings>

Wild7::Wild7(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Wild7)
{
    ui->setupUi(this);

    setupModel();
    updateProfiles();

    qRegisterMetaType<QVector<WildResult>>("QVector<WildResult>");
}

Wild7::~Wild7()
{
    QSettings settings;
    settings.setValue("wild7/geometry", this->saveGeometry());

    delete ui;
}

void Wild7::updateProfiles()
{
    profiles = Profile7::loadProfileList();
    profiles.insert(profiles.begin(), Profile7());

    QSettings setting;
    int val = setting.value("wild7/profile", 0).toInt();

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

void Wild7::setupModel()
{
    model = new WildModel(ui->tableViewWild);

    ui->tableViewWild->setModel(model);

    auto natures = Utility::getNatures();
    auto hiddenPowers = Utility::getHiddenPowers();
    auto genderRatios = Utility::getGenderRatios();

    ui->comboBoxWildNature->addItems(natures);
    ui->comboBoxWildNature->setup();
    ui->comboBoxWildHiddenPower->addItems(hiddenPowers);
    ui->comboBoxWildHiddenPower->setup();
    ui->comboBoxWildEncounterSlot->setup();
    ui->comboBoxWildGenderRatio->addItems(genderRatios);
    ui->comboBoxWildSynchNature->addItems(natures);

    ui->comboBoxWildGenderRatio->setItemData(0, 0);
    ui->comboBoxWildGenderRatio->setItemData(1, 126);
    ui->comboBoxWildGenderRatio->setItemData(2, 30);
    ui->comboBoxWildGenderRatio->setItemData(3, 62);
    ui->comboBoxWildGenderRatio->setItemData(4, 190);
    ui->comboBoxWildGenderRatio->setItemData(5, 224);
    ui->comboBoxWildGenderRatio->setItemData(6, 1);
    ui->comboBoxWildGenderRatio->setItemData(7, 2);

    ui->comboBoxWildEncounter->setItemData(0, WildType::Grass);
    ui->comboBoxWildEncounter->setItemData(1, WildType::Fish);

    ui->textBoxWildStartFrame->setValues(InputType::Frame32Bit);
    ui->textBoxWildEndFrame->setValues(InputType::Frame32Bit);

    ui->dateTimeEditWildStartDate->setCalendarPopup(true);
    ui->dateTimeEditWildEndDate->setCalendarPopup(true);

    QDateTime dt(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEditWildStartDate->setMinimumDateTime(dt);
    ui->dateTimeEditWildEndDate->setMinimumDateTime(dt);

    connect(ui->pushButtonWildSearch, &QPushButton::clicked, this, &Wild7::search);
    connect(ui->pushButtonProfileManager, &QPushButton::clicked, this, &Wild7::profileManager);
    connect(
        ui->comboBoxProfiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Wild7::profilesIndexChanged);

    QSettings setting;
    if (setting.contains("wild7/geometry"))
        this->restoreGeometry(setting.value("wild/geometry").toByteArray());
}

void Wild7::search()
{
    QDateTime start = ui->dateTimeEditWildStartDate->dateTime();
    start.setTimeSpec(Qt::UTC);
    QDateTime end = ui->dateTimeEditWildEndDate->dateTime();
    end.setTimeSpec(Qt::UTC);
    u32 frameStart = ui->textBoxWildStartFrame->getUInt();
    u32 frameEnd = ui->textBoxWildEndFrame->getUInt();

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
    ui->pushButtonWildSearch->setEnabled(false);
    ui->pushButtonWildCancel->setEnabled(true);

    QVector<u8> min = ui->ivFilterWild->getLower();
    QVector<u8> max = ui->ivFilterWild->getUpper();

    WildFilter filter(min, max, ui->comboBoxWildNature->getChecked(), ui->comboBoxWildHiddenPower->getChecked(),
        ui->comboBoxWildEncounterSlot->getChecked(), ui->comboBoxWildAbility->currentIndex() - 1,
        ui->checkBoxWildShiny->isChecked(), ui->comboBoxWildGender->currentIndex());

    auto *search = new WildSearcher7(start, end, frameStart, frameEnd, ui->checkBoxWildSynch->isChecked(),
        ui->comboBoxWildSynchNature->currentIndex(),
        static_cast<WildType>(ui->comboBoxWildEncounter->currentData().toInt()),
        ui->comboBoxWildGenderRatio->currentData().toInt(), profiles.at(ui->comboBoxProfiles->currentIndex()), filter);

    connect(search, &WildSearcher7::finished, this, [=] {
        ui->pushButtonWildSearch->setEnabled(true);
        ui->pushButtonWildCancel->setEnabled(false);
    });
    connect(search, &WildSearcher7::updateProgress, this, &Wild7::update);
    connect(ui->pushButtonWildCancel, &QPushButton::clicked, search, &WildSearcher7::cancelSearch);

    ui->progressBarWild->setValue(0);
    ui->progressBarWild->setMaximum(search->maxProgress());

    search->startSearch();
}

void Wild7::update(const QVector<WildResult> &frames, int val)
{
    model->addItems(frames);
    ui->progressBarWild->setValue(val);
}

void Wild7::profileManager()
{
    auto *manager = new ProfileManager7();
    connect(manager, &ProfileManager7::updateProfiles, [=]() { emit alertProfiles(7); });
    manager->show();
    manager->raise();
}

void Wild7::profilesIndexChanged(int index)
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
        setting.setValue("wild7/profile", index);
    }
}
