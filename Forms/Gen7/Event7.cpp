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

#include "Event7.hpp"
#include "ui_Event7.h"
#include <Core/Gen7/EventSearcher7.hpp>
#include <Core/Util/Utility.hpp>
#include <Forms/Gen7/ProfileManager7.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

Event7::Event7(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Event7)
{
    ui->setupUi(this);

    setupModel();
    updateProfiles();

    qRegisterMetaType<QVector<EventResult>>("QVector<EventResult>");
}

Event7::~Event7()
{
    QSettings settings;
    settings.setValue("event7/geometry", this->saveGeometry());

    delete ui;
}

void Event7::updateProfiles()
{
    profiles = Profile7::loadProfileList();
    profiles.insert(profiles.begin(), Profile7());

    QSettings setting;
    int val = setting.value("event7/profile", 0).toInt();

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

void Event7::setupModel()
{
    model = new EventModel(ui->tableViewEvent);

    ui->tableViewEvent->setModel(model);

    auto natures = Utility::getNatures();
    auto hiddenPowers = Utility::getHiddenPowers();

    ui->comboBoxEventNature->addItems(natures);
    ui->comboBoxEventNature->setup();
    ui->comboBoxEventNatureLocked->addItems(natures);
    ui->comboBoxEventHiddenPower->addItems(hiddenPowers);
    ui->comboBoxEventHiddenPower->setup();

    ui->labelEventPID->setVisible(false);
    ui->textBoxEventPID->setVisible(false);
    ui->labelEventEC->setVisible(false);
    ui->textBoxEventEC->setVisible(false);
    ui->comboBoxEventPIDType->setItemData(0, PIDType::Random);
    ui->comboBoxEventPIDType->setItemData(1, PIDType::Nonshiny);
    ui->comboBoxEventPIDType->setItemData(2, PIDType::Shiny);
    ui->comboBoxEventPIDType->setItemData(3, PIDType::Specified);

    ui->textBoxEventStartFrame->setValues(InputType::Frame32Bit);
    ui->textBoxEventEndFrame->setValues(InputType::Frame32Bit);
    ui->textBoxEventTID->setValues(InputType::ID);
    ui->textBoxEventSID->setValues(InputType::ID);
    ui->textBoxEventPID->setValues(InputType::Seed32Bit);
    ui->textBoxEventEC->setValues(InputType::Seed32Bit);

    ui->dateTimeEditEventStartDate->setCalendarPopup(true);
    ui->dateTimeEditEventEndDate->setCalendarPopup(true);

    QDateTime dt(QDate(2000, 1, 1), QTime(0, 0, 0));
    ui->dateTimeEditEventStartDate->setMinimumDateTime(dt);
    ui->dateTimeEditEventEndDate->setMinimumDateTime(dt);

    QSettings setting;
    if (setting.contains("event7/geometry"))
        this->restoreGeometry(setting.value("event7/geometry").toByteArray());
}

void Event7::search()
{
    QDateTime start = ui->dateTimeEditEventStartDate->dateTime();
    start.setTimeSpec(Qt::UTC);
    QDateTime end = ui->dateTimeEditEventEndDate->dateTime();
    start.setTimeSpec(Qt::UTC);
    u32 frameStart = ui->textBoxEventStartFrame->getUInt();
    u32 frameEnd = ui->textBoxEventEndFrame->getUInt();

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
    ui->pushButtonEventSearch->setEnabled(false);
    ui->pushButtonEventCancel->setEnabled(true);

    QVector<u8> min = ui->ivFilterEvent->getLower();
    QVector<u8> max = ui->ivFilterEvent->getUpper();

    int ivCount = ui->spinBoxEventRandomIVs->value();
    PIDType type = static_cast<PIDType>(ui->comboBoxEventPIDType->currentData().toInt());

    EventFilter filter(min, max, ui->comboBoxEventNature->getChecked(), ui->comboBoxEventHiddenPower->getChecked(),
        ui->comboBoxEventAbility->currentIndex() - 1, ui->checkBoxEventShiny->isChecked(),
        ui->comboBoxEventGender->currentIndex());

    QVector<u8> ivTemplate;
    ivTemplate.append(ui->checkBoxEventHP->isChecked() ? ui->spinBoxEventHP->value() : -1);
    ivTemplate.append(ui->checkBoxEventAtk->isChecked() ? ui->spinBoxEventAtk->value() : -1);
    ivTemplate.append(ui->checkBoxEventDef->isChecked() ? ui->spinBoxEventDef->value() : -1);
    ivTemplate.append(ui->checkBoxEventSpA->isChecked() ? ui->spinBoxEventSpA->value() : -1);
    ivTemplate.append(ui->checkBoxEventSpD->isChecked() ? ui->spinBoxEventSpD->value() : -1);
    ivTemplate.append(ui->checkBoxEventSpe->isChecked() ? ui->spinBoxEventSpe->value() : -1);

    auto *search = new EventSearcher7(
        start, end, frameStart, frameEnd, ivCount, type, profiles.at(ui->comboBoxProfiles->currentIndex()), filter);
    search->setLocks(ui->checkBoxEventAbilityLock->isChecked(), ui->comboBoxEventAbilityLocked->currentIndex(),
        ui->checkBoxEventNatureLocked->isChecked(), ui->comboBoxEventNatureLocked->currentIndex(),
        ui->checkBoxEventGenderLocked->isChecked(), ui->comboBoxEventGender->currentIndex());
    search->setIDs(ui->checkBoxEventOtherInfo->isChecked(), ui->textBoxEventTID->getUShort(),
        ui->textBoxEventSID->getUShort(), ui->checkBoxEventYourID->isChecked());
    search->setHidden(ui->textBoxEventPID->getUInt(), ui->textBoxEventEC->getUInt());
    search->setIVTemplate(ivTemplate);

    connect(search, &EventSearcher7::finished, this, [=] {
        ui->pushButtonEventSearch->setEnabled(true);
        ui->pushButtonEventCancel->setEnabled(false);
    });
    connect(search, &EventSearcher7::updateProgress, this, &Event7::update);
    connect(ui->pushButtonEventCancel, &QPushButton::clicked, search, &EventSearcher7::cancelSearch);

    ui->progressBarEvent->setValue(0);
    ui->progressBarEvent->setMaximum(search->maxProgress());

    search->startSearch();
}

void Event7::update(const QVector<EventResult> &frames, int val)
{
    model->addItems(frames);
    ui->progressBarEvent->setValue(val);
}

void Event7::profileManager()
{
    auto *manager = new ProfileManager7();
    connect(manager, &ProfileManager7::updateProfiles, [=]() { emit alertProfiles(7); });
    manager->show();
    manager->raise();
}

void Event7::profilesIndexChanged(int index)
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
        setting.setValue("event7/profile", index);
    }
}

void Event7::checkBoxEventAbilityLock(bool checked)
{
    ui->comboBoxEventAbilityLocked->clear();
    if (checked)
    {
        ui->comboBoxEventAbilityLocked->addItem("-");
        ui->comboBoxEventAbilityLocked->addItem("1");
        ui->comboBoxEventAbilityLocked->addItem("2");
        ui->comboBoxEventAbilityLocked->addItem("H");
    }
    else
    {
        ui->comboBoxEventAbilityLocked->addItem("1/2");
        ui->comboBoxEventAbilityLocked->addItem("1/2/H");
    }
}

void Event7::comboBoxEventPIDTypeIndexChanged(int index)
{
    ui->labelEventPID->setVisible(index == 3);
    ui->textBoxEventPID->setVisible(index == 3);
}

void Event7::importEvent()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "Select a wondercard file", QDir::currentPath(), "Wondercard (*.wc7 *.wc7full)");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray data = file.readAll();
            file.close();
            if (data.size() != 784 && data.size() != 264)
            {
                QMessageBox error;
                error.setText("Invalid format for wondercard");
                error.exec();
                return;
            }

            if (data.size() == 784)
            {
                data = data.mid(520);
            }

            if (data.at(0x51) != 0)
            {
                QMessageBox error;
                error.setText("Invalid wondercard type");
                error.exec();
                return;
            }

            ui->checkBoxEventAbilityLock->setChecked(data.at(0xA2) < 3);
            checkBoxEventAbilityLock(ui->checkBoxEventAbilityLock->isChecked());
            ui->comboBoxEventAbilityLocked->setCurrentIndex(
                ui->checkBoxEventAbilityLock->isChecked() ? data.at(0xA2) + 1 : data.at(0xA2) - 3);

            ui->checkBoxEventNatureLocked->setChecked(static_cast<u8>(data.at(0xA0)) != 0xFF);
            ui->comboBoxEventNatureLocked->setCurrentIndex(
                ui->checkBoxEventNatureLocked->isChecked() ? data.at(0xA0) : 0);

            ui->checkBoxEventGenderLocked->setChecked(data.at(0xA1) != 3);
            ui->comboBoxEventGenderLocked->setCurrentIndex(
                ui->checkBoxEventGenderLocked->isChecked() ? (data.at(0xA1) + 1) % 3 : 0);

            QVector<u8> ivs;
            for (u8 order : { 0, 1, 2, 4, 5, 3 })
            {
                u8 iv = data.at(0xAF + order);
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

            ui->spinBoxEventRandomIVs->setValue(ivFlag == 0 ? 0 : ivFlag - 0xFB);

            ui->checkBoxEventHP->setChecked(ivFlag == 0 && ivs.at(0) <= 31);
            ui->spinBoxEventHP->setValue(ui->checkBoxEventHP->isChecked() ? ivs.at(0) : 0);

            ui->checkBoxEventAtk->setChecked(ivFlag == 0 && ivs.at(1) <= 31);
            ui->spinBoxEventAtk->setValue(ui->checkBoxEventAtk->isChecked() ? ivs.at(1) : 0);

            ui->checkBoxEventDef->setChecked(ivFlag == 0 && ivs.at(2) <= 31);
            ui->spinBoxEventDef->setValue(ui->checkBoxEventDef->isChecked() ? ivs.at(2) : 0);

            ui->checkBoxEventSpA->setChecked(ivFlag == 0 && ivs.at(3) <= 31);
            ui->spinBoxEventSpA->setValue(ui->checkBoxEventSpA->isChecked() ? ivs.at(3) : 0);

            ui->checkBoxEventSpD->setChecked(ivFlag == 0 && ivs.at(4) <= 31);
            ui->spinBoxEventSpD->setValue(ui->checkBoxEventSpD->isChecked() ? ivs.at(4) : 0);

            ui->checkBoxEventSpe->setChecked(ivFlag == 0 && ivs.at(5) <= 31);
            ui->spinBoxEventSpe->setValue(ui->checkBoxEventSpe->isChecked() ? ivs.at(5) : 0);

            ui->checkBoxEventOtherInfo->setChecked(true);
            u16 tid = (static_cast<u16>(data.at(0x69)) << 8) | static_cast<u8>(data.at(0x68));
            ui->textBoxEventTID->setText(QString::number(tid));
            u16 sid = (static_cast<u16>(data.at(0x6B)) << 8) | static_cast<u8>(data.at(0x6A));
            ui->textBoxEventSID->setText(QString::number(sid));

            QVector<u8> typeOrder = { 3, 0, 2, 1 };
            ui->comboBoxEventPIDType->setCurrentIndex(typeOrder.at(data.at(0xA3)));
            if (ui->comboBoxEventPIDType->currentIndex() == 3)
            {
                u32 pid = (data.at(0xD7) << 24) | (data.at(0xD6) << 16) | (data.at(0xD5) << 8) | data.at(0xD4);
                ui->textBoxEventPID->setText(QString::number(pid, 16));
            }

            u32 ec = (data.at(0x73) << 24) | (data.at(0x72) << 16) | (data.at(0x71) << 8) | data.at(0x70);
            ui->textBoxEventEC->setText(QString::number(ec, 16));
            ui->labelEventEC->setVisible(ec > 0);
            ui->textBoxEventEC->setVisible(ec > 0);

            ui->checkBoxEventYourID->setChecked(data.at(0xB5) == 3);
            ui->checkBoxEventEgg->setChecked(data.at(0xD1) == 1);
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
