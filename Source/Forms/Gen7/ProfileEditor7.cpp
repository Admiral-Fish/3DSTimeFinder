/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2021 by Admiral_Fish
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

#include "ProfileEditor7.hpp"
#include "ui_ProfileEditor7.h"
#include <QMessageBox>
#include <QSettings>

ProfileEditor7::ProfileEditor7(QWidget *parent) : QDialog(parent), ui(new Ui::ProfileEditor7)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModels();
}

ProfileEditor7::ProfileEditor7(const Profile7 &profile, QWidget *parent) : QDialog(parent), ui(new Ui::ProfileEditor7)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModels();

    ui->lineEditProfileName->setText(QString::fromStdString(profile.getName()));
    ui->comboBoxProfileVersion->setCurrentIndex(ui->comboBoxProfileVersion->findData(profile.getVersion()));
    ui->textBoxProfileOffset->setText(QString::number(profile.getOffset()));
    ui->textBoxProfileTick->setText(QString::number(profile.getTick(), 16));
    ui->textBoxProfileTID->setText(QString::number(profile.getTID()));
    ui->textBoxProfileSID->setText(QString::number(profile.getSID()));
    ui->checkBoxShinyCharm->setChecked(profile.getShinyCharm());

    original = profile;
}

ProfileEditor7::ProfileEditor7(u32 tick, u32 offset, QWidget *parent) : QDialog(parent), ui(new Ui::ProfileEditor7)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModels();

    ui->textBoxProfileOffset->setText(QString::number(offset));
    ui->textBoxProfileTick->setText(QString::number(tick, 16));
}

ProfileEditor7::~ProfileEditor7()
{
    QSettings setting;
    setting.setValue("profileEditor/geometry", this->saveGeometry());

    delete ui;
}

Profile7 ProfileEditor7::getNewProfile()
{
    return fresh;
}

Profile7 ProfileEditor7::getOriginal()
{
    return original;
}

void ProfileEditor7::setupModels()
{
    ui->textBoxProfileTID->setValues(InputType::ID);
    ui->textBoxProfileSID->setValues(InputType::ID);
    ui->textBoxProfileTick->setValues(InputType::Seed32Bit);
    ui->textBoxProfileOffset->setValues(0, 100, 3, 10);

    ui->comboBoxProfileVersion->setItemData(0, Game::Sun);
    ui->comboBoxProfileVersion->setItemData(1, Game::Moon);
    ui->comboBoxProfileVersion->setItemData(2, Game::UltraSun);
    ui->comboBoxProfileVersion->setItemData(3, Game::UltraMoon);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ProfileEditor7::accepted);

    QSettings setting;
    if (setting.contains("profileEditor/geometry"))
    {
        this->restoreGeometry(setting.value("profileEditor/geometry").toByteArray());
    }
}

void ProfileEditor7::accepted()
{
    QString input = ui->lineEditProfileName->text().trimmed();
    if (input.isEmpty())
    {
        QMessageBox error;
        error.setText("Enter a Profile Name.");
        error.exec();
        return;
    }

    fresh = Profile7(input.toStdString(), ui->textBoxProfileOffset->getUInt(), ui->textBoxProfileTick->getUInt(),
                     ui->textBoxProfileTID->getUShort(), ui->textBoxProfileSID->getUShort(),
                     static_cast<Game>(ui->comboBoxProfileVersion->currentData().toInt()), ui->checkBoxShinyCharm->isChecked());

    done(QDialog::Accepted);
}
