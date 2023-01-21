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

#ifndef SUBMASTERWIDGET_H
#define SUBMASTERWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include <QComboBox>
//#include <QLayout>
#include "../qontrejour.h"
#include "submasterslider.h"

// sliders are created in mainwindow and pass to widget

class SubMasterWidget
    : public QWidget
{

  Q_OBJECT

public:

  explicit SubMasterWidget(QWidget *parent = nullptr);

  // getters
  QList<SubMasterSlider *> getL_sliders() const { return m_L_sliders; }

  // setters
  void setL_sliders(const QList<SubMasterSlider *> &t_L_sliders);

  void clear();

  //  QSize sizeHint() const override;

signals:

public slots:

  void populateWidget();

private:

  QStackedLayout *m_stackedLayout;
  QComboBox *m_changePageComboBox;
//  QList<QWidget *> m_L_widget;

  QList<SubMasterSlider *> m_L_sliders;

};

#endif // SUBMASTERWIDGET_H
