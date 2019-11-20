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

#include "ProfileEditor6.hpp"
#include "ui_ProfileEditor6.h"
#include <QMessageBox>
#include <QSettings>

ProfileEditor6::ProfileEditor6(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProfileEditor6)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModels();
}

ProfileEditor6::ProfileEditor6(const Profile6 &profile, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProfileEditor6)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModels();

    ui->lineEditProfileName->setText(profile.getName());
    ui->comboBoxProfileVersion->setCurrentIndex(ui->comboBoxProfileVersion->findData(profile.getVersion()));
    ui->textBoxProfileTID->setText(QString::number(profile.getTID()));
    ui->textBoxProfileSID->setText(QString::number(profile.getSID()));
    ui->textBoxProfileSaveVariable->setText(QString::number(profile.getSaveVariable(), 16));
    ui->textBoxProfileTimeVariable->setText(QString::number(profile.getTimeVariable(), 16));
    ui->checkBoxShinyCharm->setChecked(profile.getShinyCharm());

    original = profile;
}

ProfileEditor6::~ProfileEditor6()
{
    QSettings setting;
    setting.setValue("profileEditor/geometry", this->saveGeometry());

    delete ui;
}

Profile6 ProfileEditor6::getNewProfile()
{
    return fresh;
}

Profile6 ProfileEditor6::getOriginal()
{
    return original;
}

void ProfileEditor6::setupModels()
{
    ui->textBoxProfileTID->setValues(InputType::ID);
    ui->textBoxProfileSID->setValues(InputType::ID);
    ui->textBoxProfileSaveVariable->setValues(InputType::Seed32Bit);
    ui->textBoxProfileTimeVariable->setValues(InputType::Seed32Bit);

    ui->comboBoxProfileVersion->setItemData(0, Game::X);
    ui->comboBoxProfileVersion->setItemData(1, Game::Y);
    ui->comboBoxProfileVersion->setItemData(2, Game::OR);
    ui->comboBoxProfileVersion->setItemData(3, Game::AS);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ProfileEditor6::accepted);

    QSettings setting;
    if (setting.contains("profileEditor/geometry"))
        this->restoreGeometry(setting.value("profileEditor/geometry").toByteArray());
}

void ProfileEditor6::accepted()
{
    QString input = ui->lineEditProfileName->text().trimmed();
    if (input.isEmpty())
    {
        QMessageBox error;
        error.setText(tr("Enter a Profile Name."));
        error.exec();
        return;
    }

    fresh = Profile6(input, ui->textBoxProfileSaveVariable->getUInt(), ui->textBoxProfileTimeVariable->getUInt(),
        ui->textBoxProfileTID->getUShort(), ui->textBoxProfileSID->getUShort(),
        static_cast<Game>(ui->comboBoxProfileVersion->currentData().toInt()), ui->checkBoxShinyCharm->isChecked());

    done(QDialog::Accepted);
}
