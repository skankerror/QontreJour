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

#ifndef VALUESLIDERSWIDGET_H
#define VALUESLIDERSWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include <QComboBox>
#include "../core/dmxvalue.h"

class DmxValueSlider;

class ValueSlidersWidget
    : public QWidget
{

  Q_OBJECT

public :

  explicit ValueSlidersWidget(QWidget *parent = nullptr);

  // getters
  QList<DmxValueSlider *> getL_sliders() const { return m_L_sliders; }
  DmxValue *getRootValue() const{ return m_rootValue; }

  // setters
  void setL_sliders(const QList<DmxValueSlider *> &t_L_sliders);

public slots :

  virtual void populateWidget() = 0;

  void setRootValue(DmxValue *t_rootValue)
  { m_rootValue = t_rootValue; }

protected :

  QStackedLayout *m_stackedLayout;
  QComboBox *m_changePageComboBox;

  DmxValue *m_rootValue;
  QList<DmxValueSlider *> m_L_sliders;

};

/**********************************************************************/

class DirectChannelWidget
    : public ValueSlidersWidget
{

  Q_OBJECT

public :

  explicit DirectChannelWidget(QWidget *parent = nullptr);

  void populateWidget() override;

};

/*************************************************************************/

class SubmasterWidget
    : public ValueSlidersWidget
{

  Q_OBJECT

public :

  explicit SubmasterWidget(QWidget *parent = nullptr);

public slots:

  void populateWidget() override;

};

/*************************************************************************/

class DmxValueSlider
    : public QSlider
{

  Q_OBJECT

public :

  explicit DmxValueSlider(QWidget *parent = nullptr);

  DmxValueSlider(DmxValue *t_dmxValue,
                 QWidget *parent = nullptr);

  virtual ~DmxValueSlider();

  DmxValue *getDmxValue() const{ return m_dmxValue; }
  void setDmxValue(DmxValue *t_dmxValue){ m_dmxValue = t_dmxValue; }

public slots :

  void unMoveSlider(dmx t_level);

protected slots :

  virtual void updateLevel(int t_level);
  virtual void onValueLevelChanged(DmxValue::SignalSenderType t_type,
                                   dmx t_level);

protected :

  DmxValue *m_dmxValue;
};


#endif // VALUESLIDERSWIDGET_H
