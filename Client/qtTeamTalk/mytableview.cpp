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

    initializeAccessibility();
}

void MyTableView::initializeAccessibility()
{
    QAccessible::installFactory([](const QString &className, QObject *object) -> QAccessibleInterface* {
        if (className == QLatin1String("QTableView") && qobject_cast<QTableView*>(object))
        {
            return new CustomAccessibleTable(qobject_cast<QTableView*>(object));
        }
        return nullptr;
    });
}

void MyTableView::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Space)
    {
        QModelIndex currentIndex = this->currentIndex();
        if (currentIndex.isValid())
        {
            emit this->doubleClicked(currentIndex);
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
    else if (e->key() == Qt::Key_Home && e->modifiers() & Qt::ControlModifier)
    {
        int currentColumn = this->currentIndex().column();
        QModelIndex firstRowIndex = this->model()->index(0, currentColumn);
        if (firstRowIndex.isValid())
        {
            this->setCurrentIndex(firstRowIndex);
        }
    }
    else if (e->key() == Qt::Key_End && e->modifiers() & Qt::ControlModifier)
    {
        int currentColumn = this->currentIndex().column();
        int lastRow = this->model()->rowCount() - 1;
        QModelIndex lastRowIndex = this->model()->index(lastRow, currentColumn);
        if (lastRowIndex.isValid())
        {
            this->setCurrentIndex(lastRowIndex);
        }
    }
    else
    {
        QTableView::keyPressEvent(e);
    }
}

void MyTableView::moveColumnLeft()
{
    if (!this->horizontalHeader()->sectionsMovable())
        return;

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
    if (!this->horizontalHeader()->sectionsMovable())
        return;

    int col = this->currentIndex().column();
    int visualIndex = this->horizontalHeader()->visualIndex(col);
    if (visualIndex < this->model()->columnCount() - 1)
    {
        this->horizontalHeader()->moveSection(visualIndex, visualIndex + 1);
        this->setCurrentIndex(this->model()->index(this->currentIndex().row(), this->horizontalHeader()->logicalIndex(visualIndex + 1)));
    }
}

int CustomAccessibleTable::rowCount() const
{
    return m_tableView->model()->rowCount();
}

int CustomAccessibleTable::columnCount() const
{
    return m_tableView->model()->columnCount();
}

QAccessibleInterface* CustomAccessibleTable::cellAt(int row, int column) const
{
    if (row < 0 || column < 0 || row >= rowCount() || column >= columnCount())
        return nullptr;

    QModelIndex index = m_tableView->model()->index(row, column);
    if (!index.isValid())
        return nullptr;

    return QAccessible::queryAccessibleInterface(m_tableView->viewport());
}

// Implémentation des méthodes nécessaires pour QAccessibleInterface
QRect CustomAccessibleTable::rect() const
{
    return m_tableView->rect();
}

QAccessibleInterface* CustomAccessibleTable::childAt(int x, int y) const
{
    QModelIndex index = m_tableView->indexAt(QPoint(x, y));
    if (!index.isValid())
        return nullptr;

    return cellAt(index.row(), index.column());
}

int CustomAccessibleTable::navigate(QAccessible::RelationFlag /*rel*/, int /*entry*/, QAccessibleInterface** /*target*/) const
{
    return -1; // Implémentation simple, peut être améliorée si nécessaire
}

int CustomAccessibleTable::indexOfChild(const QAccessibleInterface* child) const
{
    return -1; // Implémentation simple, peut être améliorée si nécessaire
}

QAccessibleInterface* CustomAccessibleTable::focusChild() const
{
    return nullptr; // Implémentation simple, peut être améliorée si nécessaire
}

QAccessibleInterface* CustomAccessibleTable::parent() const
{
    return QAccessible::queryAccessibleInterface(m_tableView->parentWidget());
}

QAccessible::Role CustomAccessibleTable::role() const
{
    return QAccessible::Table;
}

QAccessible::State CustomAccessibleTable::state() const
{
    return QAccessible::State();
}

QString CustomAccessibleTable::text(QAccessible::Text t) const
{
    switch (t)
    {
    case QAccessible::Name:
        return m_tableView->objectName();
    default:
        return QString();
    }
}

void CustomAccessibleTable::setText(QAccessible::Text t, const QString& text)
{
    if (t == QAccessible::Name)
        m_tableView->setObjectName(text);
}

bool CustomAccessibleTable::isValid() const
{
    return m_tableView != nullptr;
}

QObject* CustomAccessibleTable::object() const
{
    return m_tableView;
}

QAccessibleInterface* CustomAccessibleTable::child(int index) const
{
    if (index < 0 || index >= childCount())
        return nullptr;

    int row = index / columnCount();
    int col = index % columnCount();
    return cellAt(row, col);
}

int CustomAccessibleTable::childCount() const
{
    return rowCount() * columnCount();
}
