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

#include "ProfileManager6.hpp"
#include "ui_ProfileManager6.h"
#include <Core/Parents/ProfileLoader.hpp>
#include <Forms/Gen6/ProfileEditor6.hpp>
#include <Forms/Models/ProfileModel6.hpp>
#include <QMessageBox>
#include <QSettings>

ProfileManager6::ProfileManager6(QWidget *parent) : QWidget(parent), ui(new Ui::ProfileManager6)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose);

    setupModels();
}

ProfileManager6::~ProfileManager6()
{
    QSettings setting;
    setting.setValue("profileManager/geometry", this->saveGeometry());

    delete ui;
}

void ProfileManager6::setupModels()
{
    model = new ProfileModel6(ui->tableView);
    model->addItems(ProfileLoader6::getProfiles());
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->pushButtonNew, &QPushButton::clicked, this, &ProfileManager6::create);
    connect(ui->pushButtonEdit, &QPushButton::clicked, this, &ProfileManager6::edit);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &ProfileManager6::remove);

    QSettings setting;
    if (setting.contains("profileManager/geometry"))
    {
        this->restoreGeometry(setting.value("profileManager/geometry").toByteArray());
    }
}

void ProfileManager6::create()
{
    QScopedPointer<ProfileEditor6> dialog(new ProfileEditor6);
    if (dialog->exec() == QDialog::Accepted)
    {
        Profile6 profile = dialog->getNewProfile();
        ProfileLoader6::addProfile(profile);
        model->addItem(profile);
        emit updateProfiles();
    }
}

void ProfileManager6::edit()
{
    int r = ui->tableView->currentIndex().row();

    if (r < 0)
    {
        QMessageBox error;
        error.setText("Please select a profile.");
        error.exec();
        return;
    }

    QScopedPointer<ProfileEditor6> dialog(new ProfileEditor6(model->getItem(r)));
    if (dialog->exec() == QDialog::Accepted)
    {
        Profile6 profile = dialog->getNewProfile();
        ProfileLoader6::updateProfile(profile, dialog->getOriginal());
        model->updateItem(profile, r);
        emit updateProfiles();
    }
}

void ProfileManager6::remove()
{
    int r = ui->tableView->currentIndex().row();

    if (r < 0)
    {
        QMessageBox error;
        error.setText("Please select a profile.");
        error.exec();
        return;
    }

    QMessageBox message(QMessageBox::Question, "Delete profile", "Are you sure you wish to delete this profile?",
                        QMessageBox::Yes | QMessageBox::No);
    if (message.exec() == QMessageBox::Yes)
    {
        ProfileLoader6::removeProfile(model->getItem(r));
        model->removeItem(r);
        emit updateProfiles();
    }
}
