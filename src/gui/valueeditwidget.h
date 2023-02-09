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
#include <QLabel>
#include <QSpinBox>
#include "../core/dmxvalue.h"
#include "../core/dmxscene.h"

class GroupEditWidget;

class ValueEditWidget
    : public QWidget
{

  Q_OBJECT

public :

  explicit ValueEditWidget(DmxValue *t_rootValue,
                           QWidget *parent = nullptr);

private :

  void createGroupEditWidget();

signals :

private :

  QHBoxLayout *m_layout;
  DmxValue *m_rootValue;

};

/**********************************************************************/

class GroupEditWidget
    : public QWidget
{

  Q_OBJECT

public :

  explicit GroupEditWidget(DmxValue *t_rootValue,
                           QWidget *parent = nullptr);

private :

  DmxValue *m_rootValue;

};

#endif // VALUEEDITWIDGET_H

