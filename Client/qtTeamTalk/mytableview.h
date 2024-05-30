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

#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>
#include <QHeaderView>
#include <QKeyEvent>
#include <QAccessibleInterface>

class MyTableView : public QTableView
{
    Q_OBJECT

public:
    MyTableView(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* e) override;

private:
    void moveColumnLeft();
    void moveColumnRight();
    void initializeAccessibility();
};

class CustomAccessibleTable : public QAccessibleInterface
{
public:
    CustomAccessibleTable(QTableView* tableView)
        : m_tableView(tableView) {}

    int rowCount() const;
    int columnCount() const;
    QAccessibleInterface* cellAt(int row, int column) const;

    // Implémentations nécessaires pour QAccessibleInterface
    QRect rect() const override;
    QAccessibleInterface* childAt(int x, int y) const override;
    int navigate(QAccessible::RelationFlag rel, int entry, QAccessibleInterface** target) const; // Supprimer le spécificateur override
    int indexOfChild(const QAccessibleInterface* child) const override;
    QAccessibleInterface* focusChild() const override;
    QAccessibleInterface* parent() const override;
    QAccessible::Role role() const override;
    QAccessible::State state() const override;
    QString text(QAccessible::Text t) const override;
    void setText(QAccessible::Text t, const QString& text) override;

    bool isValid() const override;
    QObject* object() const override;
    QAccessibleInterface* child(int index) const override;
    int childCount() const override;

private:
    QTableView* m_tableView;
};
#endif // MYTABLEVIEW_H
