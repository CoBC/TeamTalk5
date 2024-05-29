/*
 * Copyright (C) 2023, Bjørn D. Rasmussen, BearWare.dk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mytableview.h"
#include <QAccessible>
#include <QShortcut>

MyTableView::MyTableView(QWidget* parent/* = nullptr*/) : QTableView(parent)
{
    QShortcut* shortcutLeft = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Left), this);
    connect(shortcutLeft, &QShortcut::activated, this, &MyTableView::moveColumnLeft);
    QShortcut* shortcutRight = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Right), this);
    connect(shortcutRight, &QShortcut::activated, this, &MyTableView::moveColumnRight);
}

void MyTableView::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Space)
    {
        QModelIndex currentIndex = this->currentIndex();
        if (currentIndex.isValid())
        {
            emit this->doubleClicked(currentIndex);
//            QAccessible::updateAccessibility(this, currentIndex, QAccessible::ValueChanged);
            QVariant value = currentIndex.data(Qt::CheckStateRole);
            if (value.isValid() && value.canConvert<Qt::CheckState>())
            {
                Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
                state = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
                this->model()->setData(currentIndex, state, Qt::CheckStateRole);
                QAccessibleValueChangeEvent ev(this->viewport(), state);
                QAccessible::updateAccessibility(&ev);
            }
            this->setCurrentIndex(currentIndex);
        }
    }
    else
    {
        QTableView::keyPressEvent(e);
    }
}

/*void MyTableView::moveColumnLeft()
{
    int col = this->currentIndex().column();
    int visualIndex = this->horizontalHeader()->visualIndex(col);
    if (visualIndex > 0)
    {
        this->horizontalHeader()->moveSection(visualIndex, visualIndex - 1);
        this->setCurrentIndex(m_proxyModel->index(ui.tableView->currentIndex().row(), ui.tableView->horizontalHeader()->logicalIndex(visualIndex - 1)));
    }
}

void MyTableView::moveColumnRight()
{
    int col = this->currentIndex().column();
    int visualIndex = this->horizontalHeader()->visualIndex(col);
    if (visualIndex < this->model()->columnCount() - 1)
    {
        this->horizontalHeader()->moveSection(visualIndex, visualIndex + 1);
        this->setCurrentIndex(m_proxyModel->index(ui.tableView->currentIndex().row(), ui.tableView->horizontalHeader()->logicalIndex(visualIndex + 1)));
    }
}*/

void MyTableView::moveColumnLeft()
{
    int col = this->currentIndex().column();
    int visualIndex = this->horizontalHeader()->visualIndex(col);
    if (visualIndex > 0)
    {
        this->horizontalHeader()->moveSection(visualIndex, visualIndex - 1);
        this->setCurrentIndex(this->model()->index(this->currentIndex().row(), this->horizontalHeader()->logicalIndex(visualIndex - 1)));
    }
}

void MyTableView::moveColumnRight()
{
    int col = this->currentIndex().column();
    int visualIndex = this->horizontalHeader()->visualIndex(col);
    if (visualIndex < this->model()->columnCount() - 1)
    {
        this->horizontalHeader()->moveSection(visualIndex, visualIndex + 1);
        this->setCurrentIndex(this->model()->index(this->currentIndex().row(), this->horizontalHeader()->logicalIndex(visualIndex + 1)));
    }
}
