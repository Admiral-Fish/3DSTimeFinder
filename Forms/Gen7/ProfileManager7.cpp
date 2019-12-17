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

#include "ProfileManager7.hpp"
#include "ui_ProfileManager7.h"
#include <Forms/Gen7/ProfileEditor7.hpp>
#include <Models/ProfileModel7.hpp>
#include <QMessageBox>
#include <QSettings>

ProfileManager7::ProfileManager7(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProfileManager7)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose);

    setupModels();
}

ProfileManager7::~ProfileManager7()
{
    QSettings setting;
    setting.setValue("profileManager/geometry", this->saveGeometry());

    delete ui;
}

void ProfileManager7::setupModels()
{
    model = new ProfileModel7(ui->tableView);
    model->addItems(Profile7::loadProfileList());
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->pushButtonNew, &QPushButton::clicked, this, &ProfileManager7::create);
    connect(ui->pushButtonEdit, &QPushButton::clicked, this, &ProfileManager7::edit);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &ProfileManager7::remove);

    QSettings setting;
    if (setting.contains("profileManager/geometry"))
        this->restoreGeometry(setting.value("profileManager/geometry").toByteArray());
}

void ProfileManager7::create()
{
    QScopedPointer<ProfileEditor7> dialog(new ProfileEditor7);
    if (dialog->exec() == QDialog::Accepted)
    {
        Profile7 profile = dialog->getNewProfile();
        profile.saveProfile();
        model->addItem(profile);
        emit updateProfiles();
    }
}

void ProfileManager7::edit()
{
    int r = ui->tableView->currentIndex().row();

    if (r < 0)
    {
        QMessageBox error;
        error.setText(tr("Please select a profile."));
        error.exec();
        return;
    }

    QScopedPointer<ProfileEditor7> dialog(new ProfileEditor7(model->getItem(r)));
    if (dialog->exec() == QDialog::Accepted)
    {
        Profile7 profile = dialog->getNewProfile();
        profile.updateProfile(dialog->getOriginal());
        model->updateItem(profile, r);
        emit updateProfiles();
    }
}

void ProfileManager7::remove()
{
    int r = ui->tableView->currentIndex().row();

    if (r < 0)
    {
        QMessageBox error;
        error.setText(tr("Please select a profile."));
        error.exec();
        return;
    }

    QMessageBox message(QMessageBox::Question, tr("Delete profile"),
        tr("Are you sure you wish to delete this profile?"), QMessageBox::Yes | QMessageBox::No);
    if (message.exec() == QMessageBox::Yes)
    {
        Profile7 profile = model->getItem(r);
        profile.deleteProfile();
        model->removeItem(r);
        emit updateProfiles();
    }
}
