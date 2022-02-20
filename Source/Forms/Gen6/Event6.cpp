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

#include "Event6.hpp"
#include "ui_Event6.h"
#include <Core/Gen6/EventSearcher6.hpp>
#include <Core/Parents/EventResult.hpp>
#include <Core/Parents/ProfileLoader.hpp>
#include <Core/Util/PIDType.hpp>
#include <Core/Util/Utility.hpp>
#include <Forms/Controls/Controls.hpp>
#include <Forms/Gen6/ProfileManager6.hpp>
#include <Forms/Models/EventModel.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QThread>
#include <QTimer>

Event6::Event6(QWidget *parent) : QWidget(parent), ui(new Ui::Event6)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModels();
    updateProfiles();

    qRegisterMetaType<QVector<EventResult>>("QVector<EventResult>");
}

Event6::~Event6()
{
    QSettings settings;
    settings.setValue("event6/geometry", this->saveGeometry());

    delete ui;
}

void Event6::updateProfiles()
{
    profiles = ProfileLoader6::getProfiles();
    profiles.insert(profiles.begin(), Profile6());

    QSettings setting;
    int val = setting.value("event6/profile", 0).toInt();

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

void Event6::setupModels()
{
    model = new EventModel(ui->tableView);

    ui->tableView->setModel(model);

    for (const auto &nature : *Utility::getNatures())
    {
        ui->comboBoxNatureLocked->addItem(QString::fromStdString(nature));
    }

    ui->filter->disableControls(Controls::EncounterSlots);

    ui->labelPID->setVisible(false);
    ui->textBoxPID->setVisible(false);
    ui->labelEC->setVisible(false);
    ui->textBoxEC->setVisible(false);

    ui->comboBoxPIDType->setup({ PIDType::Random, PIDType::Nonshiny, PIDType::Shiny, PIDType::Specified });

    ui->textBoxStartFrame->setValues(InputType::Frame32Bit);
    ui->textBoxEndFrame->setValues(InputType::Frame32Bit);
    ui->textBoxTID->setValues(InputType::ID);
    ui->textBoxSID->setValues(InputType::ID);
    ui->textBoxPID->setValues(InputType::Seed32Bit);
    ui->textBoxEC->setValues(InputType::Seed32Bit);

    ui->dateTimeEditStartDate->setCalendarPopup(true);
    ui->dateTimeEditEndDate->setCalendarPopup(true);

    QDateTime minDT(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEditStartDate->setMinimumDateTime(minDT);
    ui->dateTimeEditEndDate->setMinimumDateTime(minDT);

    QDateTime maxDT(QDate(2000, 2, 19), QTime(17, 2, 48));
    ui->dateTimeEditStartDate->setMaximumDateTime(maxDT);
    ui->dateTimeEditEndDate->setMaximumDateTime(maxDT);

    ui->dateTimeEditEndDate->setDateTime(maxDT);

    connect(ui->pushButtonProfileManager, &QPushButton::clicked, this, &Event6::profileManager);
    connect(ui->comboBoxProfiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Event6::profilesIndexChanged);
    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &Event6::search);
    connect(ui->checkBoxAbilityLock, &QCheckBox::clicked, this, &Event6::checkBoxAbilityLock);
    connect(ui->comboBoxPIDType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Event6::comboBoxPIDTypeIndexChanged);
    connect(ui->pushButtonImport, &QPushButton::clicked, this, &Event6::importEvent);

    QSettings setting;
    if (setting.contains("event6/geometry"))
    {
        this->restoreGeometry(setting.value("event6/geometry").toByteArray());
    }
}

void Event6::search()
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

    std::array<u8, 6> min = ui->filter->getMinIVs();
    std::array<u8, 6> max = ui->filter->getMaxIVs();

    u8 ivCount = static_cast<u8>(ui->spinBoxRandomIVs->value());
    auto type = static_cast<PIDType>(ui->comboBoxPIDType->getCurrentByte());

    EventFilter filter(min, max, ui->filter->getNatures(), ui->filter->getHiddenPowers(), ui->filter->getAbility(), ui->filter->getShiny(),
                       ui->filter->getGender());

    std::array<u8, 6> ivTemplate;
    ivTemplate[0] = ui->checkBoxHP->isChecked() ? static_cast<u8>(ui->spinBoxHP->value()) : 255;
    ivTemplate[1] = ui->checkBoxAtk->isChecked() ? static_cast<u8>(ui->spinBoxAtk->value()) : 255;
    ivTemplate[2] = ui->checkBoxDef->isChecked() ? static_cast<u8>(ui->spinBoxDef->value()) : 255;
    ivTemplate[3] = ui->checkBoxSpA->isChecked() ? static_cast<u8>(ui->spinBoxSpA->value()) : 255;
    ivTemplate[4] = ui->checkBoxSpD->isChecked() ? static_cast<u8>(ui->spinBoxSpD->value()) : 255;
    ivTemplate[5] = ui->checkBoxSpe->isChecked() ? static_cast<u8>(ui->spinBoxSpe->value()) : 255;

    auto *searcher
        = new EventSearcher6(start, end, frameStart, frameEnd, ivCount, type, profiles[ui->comboBoxProfiles->currentIndex()], filter);
    searcher->setLocks(ui->checkBoxAbilityLock->isChecked(), static_cast<u8>(ui->comboBoxAbilityLocked->currentIndex()),
                       ui->checkBoxNatureLocked->isChecked(), static_cast<u8>(ui->comboBoxNatureLocked->currentIndex()),
                       ui->checkBoxGenderLocked->isChecked(), static_cast<u8>(ui->comboBoxGenderLocked->currentIndex()));
    searcher->setIDs(ui->checkBoxOtherInfo->isChecked(), ui->textBoxTID->getUShort(), ui->textBoxSID->getUShort(),
                     ui->checkBoxYourID->isChecked());
    searcher->setHidden(ui->textBoxPID->getUInt(), ui->textBoxEC->getUInt());
    searcher->setIVTemplate(ivTemplate);
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

void Event6::profileManager()
{
    auto *manager = new ProfileManager6();
    connect(manager, &ProfileManager6::updateProfiles, [=]() { emit alertProfiles(6); });
    manager->show();
    manager->raise();
}

void Event6::profilesIndexChanged(int index)
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
        setting.setValue("event6/profile", index);
    }
}

void Event6::checkBoxAbilityLock(bool checked)
{
    ui->comboBoxAbilityLocked->clear();
    if (checked)
    {
        ui->comboBoxAbilityLocked->addItem("-");
        ui->comboBoxAbilityLocked->addItem("1");
        ui->comboBoxAbilityLocked->addItem("2");
        ui->comboBoxAbilityLocked->addItem("H");
    }
    else
    {
        ui->comboBoxAbilityLocked->addItem("1/2");
        ui->comboBoxAbilityLocked->addItem("1/2/H");
    }
}

void Event6::comboBoxPIDTypeIndexChanged(int index)
{
    ui->labelPID->setVisible(index == 3);
    ui->textBoxPID->setVisible(index == 3);
}

void Event6::importEvent()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select a wondercard file", QDir::currentPath(), "Wondercard (*.wc6 *.wc6full)");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray fileData = file.readAll();
            file.close();
            if (fileData.size() != 784 && fileData.size() != 264)
            {
                QMessageBox error;
                error.setText("Invalid format for wondercard");
                error.exec();
                return;
            }

            if (fileData.size() == 784)
            {
                fileData = fileData.mid(520);
            }

            auto data = fileData.data();
            if (data[0x51] != 0)
            {
                QMessageBox error;
                error.setText("Invalid wondercard type");
                error.exec();
                return;
            }

            ui->checkBoxAbilityLock->setChecked(data[0xA2] < 3);
            checkBoxAbilityLock(ui->checkBoxAbilityLock->isChecked());
            ui->comboBoxAbilityLocked->setCurrentIndex(ui->checkBoxAbilityLock->isChecked() ? data[0xA2] + 1 : data[0xA2] - 3);

            ui->checkBoxNatureLocked->setChecked(static_cast<u8>(data[0xA0]) != 0xFF);
            ui->comboBoxNatureLocked->setCurrentIndex(ui->checkBoxNatureLocked->isChecked() ? data[0xA0] : 0);

            ui->checkBoxGenderLocked->setChecked(data[0xA1] != 3);
            ui->comboBoxGenderLocked->setCurrentIndex(ui->checkBoxGenderLocked->isChecked() ? (data[0xA1] + 1) % 3 : 0);

            QVector<u8> ivs;
            for (u8 order : { 0, 1, 2, 4, 5, 3 })
            {
                u8 iv = data[0xAF + order];
                ivs.append(iv);
            }

            u8 ivFlag = 0;
            for (u8 iv : ivs)
            {
                u8 value = iv - 0xFC;
                if (value < 3)
                {
                    ivFlag = iv;
                    break;
                }
            }

            ui->spinBoxRandomIVs->setValue(ivFlag == 0 ? 0 : ivFlag - 0xFB);

            ui->checkBoxHP->setChecked(ivFlag == 0 && ivs[0] <= 31);
            ui->spinBoxHP->setValue(ui->checkBoxHP->isChecked() ? ivs[0] : 0);

            ui->checkBoxAtk->setChecked(ivFlag == 0 && ivs[1] <= 31);
            ui->spinBoxAtk->setValue(ui->checkBoxAtk->isChecked() ? ivs[1] : 0);

            ui->checkBoxDef->setChecked(ivFlag == 0 && ivs[2] <= 31);
            ui->spinBoxDef->setValue(ui->checkBoxDef->isChecked() ? ivs[2] : 0);

            ui->checkBoxSpA->setChecked(ivFlag == 0 && ivs[3] <= 31);
            ui->spinBoxSpA->setValue(ui->checkBoxSpA->isChecked() ? ivs[3] : 0);

            ui->checkBoxSpD->setChecked(ivFlag == 0 && ivs[4] <= 31);
            ui->spinBoxSpD->setValue(ui->checkBoxSpD->isChecked() ? ivs[4] : 0);

            ui->checkBoxSpe->setChecked(ivFlag == 0 && ivs[5] <= 31);
            ui->spinBoxSpe->setValue(ui->checkBoxSpe->isChecked() ? ivs[5] : 0);

            ui->checkBoxOtherInfo->setChecked(true);
            u16 tid = (static_cast<u16>(data[0x69]) << 8) | static_cast<u8>(data[0x68]);
            ui->textBoxTID->setText(QString::number(tid));
            u16 sid = (static_cast<u16>(data[0x6B]) << 8) | static_cast<u8>(data[0x6A]);
            ui->textBoxSID->setText(QString::number(sid));

            u8 typeOrder[4] = { 3, 0, 2, 1 };
            ui->comboBoxPIDType->setCurrentIndex(typeOrder[data[0xA3]]);
            if (ui->comboBoxPIDType->currentIndex() == 3)
            {
                u32 pid = (data[0xD7] << 24) | (data[0xD6] << 16) | (data[0xD5] << 8) | data[0xD4];
                ui->textBoxPID->setText(QString::number(pid, 16));
            }

            u32 ec = (data[0x73] << 24) | (data[0x72] << 16) | (data[0x71] << 8) | data[0x70];
            ui->textBoxEC->setText(QString::number(ec, 16));
            ui->labelEC->setVisible(ec > 0);
            ui->textBoxEC->setVisible(ec > 0);

            ui->checkBoxYourID->setChecked(data[0xB5] == 3);
            ui->checkBoxEgg->setChecked(data[0xD1] == 1);
        }
        else
        {
            QMessageBox error;
            error.setText("There was a problem opening the wondercard");
            error.exec();
            return;
        }
    }
}
