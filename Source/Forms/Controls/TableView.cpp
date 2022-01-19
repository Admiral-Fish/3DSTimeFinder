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

#include "TableView.hpp"
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QHeaderView>
#include <QMouseEvent>
#include <QTextStream>

TableView::TableView(QWidget *parent) : QTableView(parent)
{
}

void TableView::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);

    QHeaderView *header = this->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    for (int column = 0; column < header->count(); column++)
    {
        int width = header->sectionSize(column);
        header->setSectionResizeMode(column, QHeaderView::Interactive);
        header->resizeSection(column, width);
    }
}

void TableView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event && event->type() == QMouseEvent::MouseButtonDblClick)
    {
        setSelectionToClipBoard();
    }
}

void TableView::keyPressEvent(QKeyEvent *event)
{
    QTableView::keyPressEvent(event);

    if (event && (event->key() == Qt::Key_C) && (event->modifiers() == Qt::ControlModifier))
    {
        setSelectionToClipBoard();
    }
}

void TableView::outputModelTXT()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Output to TXT", QDir::currentPath(), "Text File (*.txt);;All Files (*)");

    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QAbstractItemModel *model = this->model();

        QTextStream ts(&file);
        int rows = model->rowCount();
        int columns = model->columnCount();

        QString header = "";
        for (int i = 0; i < columns; i++)
        {
            header += model->headerData(i, Qt::Horizontal, 0).toString();
            if (i != columns - 1)
            {
                header += "\t";
            }
        }
        header += "\n";
        ts << header;

        for (int i = 0; i < rows; i++)
        {
            QString body = "";
            for (int j = 0; j < columns; j++)
            {
                QString entry = model->data(model->index(i, j)).toString();
                body += (entry.isEmpty() ? "-" : entry);
                if (i != columns - 1)
                {
                    body += "\t";
                }
            }
            if (i != rows - 1)
            {
                body += "\n";
            }
            ts << body;
        }

        file.close();
    }
}

void TableView::outputModelCSV()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Output to CSV", QDir::currentPath(), "CSV File (*.csv);;All Files (*)");

    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QAbstractItemModel *model = this->model();

        QTextStream ts(&file);
        int rows = model->rowCount();
        int columns = model->columnCount();

        QString header = "";
        for (int i = 0; i < columns; i++)
        {
            header += model->headerData(i, Qt::Horizontal, 0).toString();
            if (i != columns - 1)
            {
                header += ",";
            }
        }
        header += "\n";
        ts << header;

        for (int i = 0; i < rows; i++)
        {
            QString body = "";
            for (int j = 0; j < columns; j++)
            {
                QString entry = model->data(model->index(i, j)).toString();
                body += (entry.isEmpty() ? "-" : entry);
                if (j != columns - 1)
                {
                    body += ",";
                }
            }
            if (i != rows - 1)
            {
                body += "\n";
            }
            ts << body;
        }

        file.close();
    }
}

void TableView::setSelectionToClipBoard()
{
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    if (!indexes.isEmpty())
    {
        QString selectedText;

        for (auto i = 0; i < indexes.size(); i++)
        {
            QModelIndex current = indexes[i];
            QString text = current.data().toString();

            if (i + 1 < selectedIndexes().count())
            {
                QModelIndex next = indexes[i + 1];

                if (next.row() != current.row())
                {
                    text += "\n";
                }
                else
                {
                    text += "\t";
                }
            }
            selectedText += text;
        }

        QApplication::clipboard()->setText(selectedText);
    }
}
