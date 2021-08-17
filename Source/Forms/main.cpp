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

#include <Forms/MainWindow.hpp>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("3DSTimeFinder");
    app.setOrganizationName("Admiral-Fish");

    QSettings setting;

    QString style = setting.value("settings/style", "dark").toString();
    if (style == "dark")
    {
        QFile file(":/qdarkstyle/dark/style.qss");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream ts(&file);
            app.setStyleSheet(ts.readAll());
            file.close();
        }
    }

    MainWindow w;
    w.show();

    return app.exec();
}
