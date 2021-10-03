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

#include "Wild7.hpp"
#include "ui_Wild7.h"
#include <Core/Gen7/WildSearcher7.hpp>
#include <Core/Parents/ProfileLoader.hpp>
#include <Forms/Gen7/ProfileManager7.hpp>
#include <Forms/Models/WildModel.hpp>
#include <QMessageBox>
#include <QSettings>
#include <QThread>
#include <QTimer>

Wild7::Wild7(QWidget *parent) : QWidget(parent), ui(new Ui::Wild7)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

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
    profiles = ProfileLoader7::getProfiles();
    profiles.insert(profiles.begin(), Profile7());

    QSettings setting;
    int val = setting.value("wild7/profile", 0).toInt();

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

void Wild7::setupModel()
{
    model = new WildModel(ui->tableView);

    ui->tableView->setModel(model);

    auto natures = Utility::getNatures();
    auto hiddenPowers = Utility::getHiddenPowers();
    auto genderRatios = Utility::getGenderRatios();

    ui->comboBoxNature->setup(natures);
    ui->comboBoxHiddenPower->setup(hiddenPowers);
    ui->comboBoxEncounterSlot->setup({});

    for (const auto &genderRatio : genderRatios)
    {
        ui->comboBoxGenderRatio->addItem(QString::fromStdString(genderRatio));
    }

    for (const auto &nature : natures)
    {
        ui->comboBoxSynchNature->addItem(QString::fromStdString(nature));
    }

    ui->comboBoxGenderRatio->setup({ 255, 127, 191, 63, 31, 0, 254 });

    ui->comboBoxGender->setup({ 255, 0, 1 });

    ui->comboBoxAbility->setup({ 255, 0, 1, 2 });

    ui->comboBoxEncounter->setup({ WildType::Grass, WildType::Fish });

    ui->textBoxStartFrame->setValues(InputType::Frame32Bit);
    ui->textBoxEndFrame->setValues(InputType::Frame32Bit);

    ui->dateTimeEditStartDate->setCalendarPopup(true);
    ui->dateTimeEditEndDate->setCalendarPopup(true);

    QDateTime dt(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEditStartDate->setMinimumDateTime(dt);
    ui->dateTimeEditEndDate->setMinimumDateTime(dt);

    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &Wild7::search);
    connect(ui->pushButtonProfileManager, &QPushButton::clicked, this, &Wild7::profileManager);
    connect(ui->comboBoxProfiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Wild7::profilesIndexChanged);

    QSettings setting;
    if (setting.contains("wild7/geometry"))
    {
        this->restoreGeometry(setting.value("wild/geometry").toByteArray());
    }
}

void Wild7::search()
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

    WildFilter filter(min, max, ui->comboBoxNature->getChecked(), ui->comboBoxHiddenPower->getChecked(),
                      ui->comboBoxEncounterSlot->getChecked(), ui->comboBoxAbility->getCurrentByte(), ui->checkBoxShiny->isChecked(),
                      ui->comboBoxGender->getCurrentByte());

    auto *searcher = new WildSearcher7(start, end, frameStart, frameEnd, ui->checkBoxSynch->isChecked(),
                                       static_cast<u8>(ui->comboBoxSynchNature->currentIndex()),
                                       static_cast<WildType>(ui->comboBoxEncounter->getCurrentByte()),
                                       ui->comboBoxGenderRatio->getCurrentByte(), profiles[ui->comboBoxProfiles->currentIndex()], filter);
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
        auto profile = profiles[index];
        ui->labelProfileOffsetValue->setText(QString::number(profile.getOffset()));
        ui->labelProfileTickValue->setText(QString::number(profile.getTick(), 16));
        ui->labelProfileTIDValue->setText(QString::number(profile.getTID()));
        ui->labelProfileSIDValue->setText(QString::number(profile.getSID()));
        ui->labelProfileGameValue->setText(QString::fromStdString(profile.getVersionString()));
        ui->labelProfileShinyCharmValue->setText(profile.getShinyCharm() ? "Yes" : "No");

        QSettings setting;
        setting.setValue("wild7/profile", index);
    }
}
