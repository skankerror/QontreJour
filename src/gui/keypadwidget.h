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

#ifndef KEYPADWIDGET_H
#define KEYPADWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include "../qontrejour.h"
#include "../core/dmxmanager.h"

class PushButton
    : public QPushButton
{
  Q_OBJECT

public :

  explicit PushButton(QWidget *parent = nullptr);
  explicit PushButton(const QString &text,
                      QWidget *parent = nullptr);
};

/****************************************************************/

class KeypadWidget
    : public QWidget
{

  Q_OBJECT

public:

  Q_ENUM(KeypadButton)

  explicit KeypadWidget(QWidget *parent = nullptr);

private :

  void populateWidget();

signals:

  void buttonClicked(KeypadButton buttonType);
};

/****************************************************************/

class PlaybackWidget
    : public QWidget
{

  Q_OBJECT

public :

  explicit PlaybackWidget(QWidget *parent = nullptr);

private :

  QSlider *m_ASlider;
  QLabel *m_ALabel;
  QSlider *m_BSlider;
  QLabel *m_BLabel;
  PushButton *m_seqInfButton;
  PushButton *m_seqSupButton;
  PushButton *m_pauseButton;
  PushButton *m_goBackButton;
  QPushButton *m_goButton;

};

/****************************************************************/

class GrandMasterWidget
    : public QWidget
{

  Q_OBJECT

public :

  explicit GrandMasterWidget(QWidget *parent = nullptr);

private :

  QSlider *m_grandMasterslider;
  QPushButton *m_blackOutButton;
  QLabel *m_grandMasterLabel;

};

#endif // KEYPADWIDGET_H
