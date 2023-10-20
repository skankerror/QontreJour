/*
 * (c) 2023 Michaël Creusy -- creusy(.)michael(@)gmail(.)com
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VALUEEDITWIDGET_H
#define VALUEEDITWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QComboBox>
#include "valuetablewidget.h"
#include "../core/dmxvalue.h"


class GroupEditWidget;

class ValueEditWidget
    : public QWidget
{

  Q_OBJECT

public :

  // cstr for edit existing group
  explicit ValueEditWidget(DmxValue *t_rootValue,
                           QWidget *parent = nullptr);

  // cstr for new group
  explicit ValueEditWidget(ValueType t_type,
                           QList<DmxValue *> t_L_value,
                           QWidget *parent = nullptr);

private :

  void createGroupEditWidget();
  void createNewGroupWidget();

private slots :

  void closeEditor();

private :

  QHBoxLayout *m_layout;
  DmxValue *m_rootValue;
  QList<DmxValue *> m_L_value;

};

/**********************************************************************/

class GroupEditWidget
    : public QWidget
{

  Q_OBJECT

public :

  // cstr for edit existing group
  explicit GroupEditWidget(DmxValue *t_rootValue,
                           QWidget *parent = nullptr);

  // cstr for new group
  explicit GroupEditWidget(QList<DmxValue *> t_L_value,
                           QWidget *parent = nullptr);

private :

  void createWidget();

signals :

  void closeEditor();

private slots :

  void okSlot();
  void cancelSlot();

private :

  DmxValue *m_rootValue;
  QList<DmxValue *> m_L_value;

  QLineEdit *m_nameEdit;
  QComboBox *m_slidersComboBox;
  ValueTableWidget *m_tableWidget;

};

#endif // VALUEEDITWIDGET_H

