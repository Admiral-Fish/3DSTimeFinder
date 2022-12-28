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

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <Forms/Gen6/Event6.hpp>
#include <Forms/Gen6/Stationary6.hpp>
#include <Forms/Gen7/Event7.hpp>
#include <Forms/Gen7/ID7.hpp>
#include <Forms/Gen7/ProfileCalibrater7.hpp>
#include <Forms/Gen7/Stationary7.hpp>
#include <Forms/Gen7/Wild7.hpp>
#include <QActionGroup>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QThread>
#include <QTimer>
#include <version.h>

MainWindow::MainWindow(bool profile, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("3DS Time Finder %1").arg(GIT_VERSION));

    setupModel();
    setupStyle();
    setupThread();

    QTimer::singleShot(1000, this, [profile] {
        if (!profile)
        {
            QMessageBox message(QMessageBox::Warning, "Unable to locate profiles",
                                "Please update path to your profiles file to restore existing profiles.");
            message.exec();
        }
    });
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("settings/style", currentStyle);
    settings.setValue("mainWindow/geometry", this->saveGeometry());

    delete ui;
}

void MainWindow::setupModel()
{
    connect(ui->pushButtonEvent6, &QPushButton::clicked, this, &MainWindow::openEvent6);
    connect(ui->pushButtonStationary6, &QPushButton::clicked, this, &MainWindow::openStationary6);
    connect(ui->pushButtonEvent7, &QPushButton::clicked, this, &MainWindow::openEvent7);
    connect(ui->pushButtonID7, &QPushButton::clicked, this, &MainWindow::openID7);
    connect(ui->pushButtonStationary7, &QPushButton::clicked, this, &MainWindow::openStationary7);
    connect(ui->pushButtonWild7, &QPushButton::clicked, this, &MainWindow::openWild7);
    connect(ui->actionProfiles, &QAction::triggered, this, &MainWindow::updateProfilePath);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::openAbout);
    connect(ui->actionCalibrator7, &QAction::triggered, this, &MainWindow::openCalibrator7);

    QSettings setting;
    if (setting.contains("mainWindow/geometry"))
    {
        this->restoreGeometry(setting.value("mainWindow/geometry").toByteArray());
    }
}

void MainWindow::setupStyle()
{
    styleGroup = new QActionGroup(ui->menuStyle);
    styleGroup->setExclusive(true);
    connect(styleGroup, &QActionGroup::triggered, this, &MainWindow::slotStyleChanged);

    QSettings setting;
    currentStyle = setting.value("settings/style", "dark").toString();

    QStringList styles = { "dark", "light" };
    for (int i = 0; i < styles.size(); i++)
    {
        const QString &style = styles[i];

        auto *action = ui->menuStyle->actions()[i];
        action->setData(style);

        if (currentStyle == style)
        {
            action->setChecked(true);
        }

        styleGroup->addAction(action);
    }
}

void MainWindow::setupThread()
{
    threadGroup = new QActionGroup(ui->menuThreads);
    threadGroup->setExclusive(true);
    connect(threadGroup, &QActionGroup::triggered, this, &MainWindow::slotThreadChanged);

    QSettings setting;

    int maxThreads = QThread::idealThreadCount();
    int selectThreads = setting.value("settings/threads", maxThreads).toInt();

    for (int i = 1; i <= maxThreads; i++)
    {
        QAction *thread = ui->menuThreads->addAction(QString::number(i));
        thread->setData(i);
        thread->setCheckable(true);

        if (i == selectThreads)
        {
            thread->setChecked(true);
        }

        threadGroup->addAction(thread);
    }
}

void MainWindow::slotStyleChanged(QAction *action)
{
    if (action)
    {
        QString style = action->data().toString();
        if (currentStyle != style)
        {
            currentStyle = style;

            QMessageBox message(QMessageBox::Question, "Style change", "Restart for changes to take effect. Restart now?",
                                QMessageBox::Yes | QMessageBox::No);
            if (message.exec() == QMessageBox::Yes)
            {
                QProcess::startDetached(QApplication::applicationFilePath());
                QApplication::quit();
            }
        }
    }
}

void MainWindow::slotThreadChanged(QAction *action)
{
    if (action)
    {
        int thread = action->data().toInt();

        QSettings setting;
        if (setting.value("settings/threads", QThread::idealThreadCount()).toInt() != thread)
        {
            setting.setValue("settings/threads", thread);
        }
    }
}

void MainWindow::updateProfilePath()
{
    QSettings setting;
    QString path = setting.value("settings/profiles").toString();

    QMessageBox box(QMessageBox::Question, "Profile Path", QString("Current profile path: %1\nWould you like to change it?").arg(path),
                    QMessageBox::Yes | QMessageBox::No);
    if (box.exec() == QMessageBox::Yes)
    {
        QString fileName = QFileDialog::getSaveFileName(this, "Select Profile json", QDir::currentPath(), "json (*.json)");
        if (!fileName.isEmpty())
        {
            if (!QFile::exists(fileName))
            {
                QFile f(fileName);
                if (!f.open(QIODevice::WriteOnly))
                {
                    QMessageBox message(QMessageBox::Information, "Profile File", "There was a problem creating the file", QMessageBox::Ok);
                    message.exec();
                    return;
                }
            }

            setting.setValue("settings/profiles", fileName);
        }
    }
}

void MainWindow::updateProfiles(int num)
{
    if (num == 6)
    {
        if (event6)
        {
            event6->updateProfiles();
        }
        if (stationary6)
        {
            stationary6->updateProfiles();
        }
    }
    else
    {
        if (event7)
        {
            event7->updateProfiles();
        }
        if (id7)
        {
            id7->updateProfiles();
        }
        if (stationary7)
        {
            stationary7->updateProfiles();
        }
        if (wild7)
        {
            wild7->updateProfiles();
        }
    }
}

void MainWindow::openEvent6()
{
    if (!event6)
    {
        event6 = new Event6();
        connect(event6, &Event6::alertProfiles, this, &MainWindow::updateProfiles);
    }
    event6->show();
    event6->raise();
}

void MainWindow::openStationary6()
{
    if (!stationary6)
    {
        stationary6 = new Stationary6();
        connect(stationary6, &Stationary6::alertProfiles, this, &MainWindow::updateProfiles);
    }
    stationary6->show();
    stationary6->raise();
}

void MainWindow::openEvent7()
{
    if (!event7)
    {
        event7 = new Event7();
        connect(event7, &Event7::alertProfiles, this, &MainWindow::updateProfiles);
    }
    event7->show();
    event7->raise();
}

void MainWindow::openID7()
{
    if (!id7)
    {
        id7 = new ID7();
        connect(id7, &ID7::alertProfiles, this, &MainWindow::updateProfiles);
    }
    id7->show();
    id7->raise();
}

void MainWindow::openStationary7()
{
    if (!stationary7)
    {
        stationary7 = new Stationary7();
        connect(stationary7, &Stationary7::alertProfiles, this, &MainWindow::updateProfiles);
    }
    stationary7->show();
    stationary7->raise();
}

void MainWindow::openWild7()
{
    if (!wild7)
    {
        wild7 = new Wild7;
        connect(wild7, &Wild7::alertProfiles, this, &MainWindow::updateProfiles);
    }
    wild7->show();
    wild7->raise();
}

void MainWindow::openAbout()
{
    QStringList info = { tr("Version: %1").arg(GIT_VERSION), tr("Branch: %1").arg(GIT_BRANCH), tr("Commit: %1").arg(GIT_COMMIT) };

    QMessageBox msg(QMessageBox::Information, tr("About"), info.join('\n'), QMessageBox::Close);
    QAbstractButton *copy = msg.addButton(tr("Copy"), QMessageBox::NoRole);
    msg.exec();
    if (msg.clickedButton() == copy)
    {
        QApplication::clipboard()->setText(info.join('\n'));
    }
    delete copy;
}

void MainWindow::openCalibrator7()
{
    auto *calibrator = new ProfileCalibrater7();
    calibrator->show();
}
