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

#include "Stationary6.hpp"
#include "ui_Stationary6.h"
#include <Core/Gen6/StationarySearcher6.hpp>
#include <Core/Parents/ProfileLoader.hpp>
#include <Core/Util/Utility.hpp>
#include <Forms/Gen6/ProfileManager6.hpp>
#include <Forms/Models/StationaryModel.hpp>
#include <QMessageBox>
#include <QSettings>
#include <QThread>
#include <QTimer>

Stationary6::Stationary6(QWidget *parent) : QWidget(parent), ui(new Ui::Stationary6)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModel();
    updateProfiles();

    qRegisterMetaType<QVector<StationaryResult>>("QVector<StationaryResult");
}

Stationary6::~Stationary6()
{
    QSettings settings;
    settings.setValue("stationary6/geometry", this->saveGeometry());

    delete ui;
}

void Stationary6::updateProfiles()
{
    profiles = ProfileLoader6::getProfiles();
    profiles.insert(profiles.begin(), Profile6());

    QSettings setting;
    int val = setting.value("stationary6/profile", 0).toInt();

    ui->comboBoxProfiles->clear();

    for (const auto &profile : profiles)
    {
        ui->comboBoxProfiles->addItem(QString::fromStdString(profile.getName()));
    }

    if (val < ui->comboBoxProfiles->count())
    {
        ui->comboBoxProfiles->setCurrentIndex(val);
    }
}

void Stationary6::setupModel()
{
    model = new StationaryModel(ui->tableView);

    ui->tableView->setModel(model);

    auto natures = Utility::getNatures();
    auto hiddenPowers = Utility::getHiddenPowers();
    auto genderRatios = Utility::getGenderRatios();

    ui->comboBoxNature->setup(natures);
    ui->comboBoxHiddenPower->setup(hiddenPowers);

    for (const auto &nature : natures)
    {
        ui->comboBoxSynchNature->addItem(QString::fromStdString(nature));
    }

    for (const auto &genderRatio : genderRatios)
    {
        ui->comboBoxGenderRatio->addItem(QString::fromStdString(genderRatio));
    }

    ui->comboBoxGenderRatio->setup({ 255, 127, 191, 63, 31, 0, 254 });

    ui->comboBoxGender->setup({ 255, 0, 1 });

    ui->comboBoxAbility->setup({ 255, 0, 1, 2 });

    ui->textBoxStartFrame->setValues(InputType::Frame32Bit);
    ui->textBoxEndFrame->setValues(InputType::Frame32Bit);

    ui->dateTimeEditStartDate->setCalendarPopup(true);
    ui->dateTimeEditEndDate->setCalendarPopup(true);

    QDateTime minDT(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEditStartDate->setMinimumDateTime(minDT);
    ui->dateTimeEditEndDate->setMinimumDateTime(minDT);

    QDateTime maxDT(QDate(2000, 2, 19), QTime(17, 2, 48));
    ui->dateTimeEditStartDate->setMaximumDateTime(maxDT);
    ui->dateTimeEditEndDate->setMaximumDateTime(maxDT);

    ui->dateTimeEditEndDate->setDateTime(maxDT);

    connect(ui->pushButtonProfileManager, &QPushButton::clicked, this, &Stationary6::profileManager);
    connect(ui->comboBoxProfiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Stationary6::profilesIndexChanged);
    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &Stationary6::search);

    QSettings setting;
    if (setting.contains("stationary6/geometry"))
    {
        this->restoreGeometry(setting.value("stationary6/geometry").toByteArray());
    }
}

void Stationary6::search()
{
    DateTime start = ui->dateTimeEditStartDate->getDateTime();
    DateTime end = ui->dateTimeEditEndDate->getDateTime();
    u32 frameStart = ui->textBoxStartFrame->getUInt();
    u32 frameEnd = ui->textBoxEndFrame->getUInt();

    if (start > end)
    {
        QMessageBox error;
        error.setText("Set end date to be after start date.");
        error.exec();
        return;
    }
    if (frameStart > frameEnd)
    {
        QMessageBox error;
        error.setText("Set end frame to be after start frame");
        error.exec();
        return;
    }

    model->clearModel();
    ui->pushButtonSearch->setEnabled(false);
    ui->pushButtonCancel->setEnabled(true);

    std::array<u8, 6> min = ui->ivFilter->getLower();
    std::array<u8, 6> max = ui->ivFilter->getUpper();

    StationaryFilter filter(min, max, ui->comboBoxNature->getChecked(), ui->comboBoxHiddenPower->getChecked(),
                            ui->comboBoxAbility->getCurrentByte(), ui->checkBoxShiny->isChecked(), ui->comboBoxGender->getCurrentByte());

    auto *searcher = new StationarySearcher6(
        start, end, frameStart, frameEnd, ui->checkBox3IVs->isChecked(),
        ui->checkBoxAbilityLock->isChecked() ? static_cast<u8>(ui->comboBoxAbilityLock->currentIndex()) : 255,
        static_cast<u8>(ui->comboBoxSynchNature->currentIndex()), ui->comboBoxGenderRatio->getCurrentByte(),
        ui->checkBoxAlwaysSynch->isChecked(), ui->checkBoxShinyLock->isChecked(), profiles[ui->comboBoxProfiles->currentIndex()], filter);
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

void Stationary6::profileManager()
{
    auto *manager = new ProfileManager6();
    connect(manager, &ProfileManager6::updateProfiles, [=]() { emit alertProfiles(6); });
    manager->show();
    manager->raise();
}

void Stationary6::profilesIndexChanged(int index)
{
    if (index >= 0)
    {
        auto profile = profiles[index];
        ui->labelProfileSaveValue->setText(QString::number(profile.getSaveVariable(), 16));
        ui->labelProfileTimeValue->setText(QString::number(profile.getTimeVariable(), 16));
        ui->labelProfileTIDValue->setText(QString::number(profile.getTID()));
        ui->labelProfileSIDValue->setText(QString::number(profile.getSID()));
        ui->labelProfileGameValue->setText(QString::fromStdString(profile.getVersionString()));
        ui->labelProfileShinyCharmValue->setText(profile.getShinyCharm() ? "Yes" : "No");

        QSettings setting;
        setting.setValue("stationary6/profile", index);
    }
}
