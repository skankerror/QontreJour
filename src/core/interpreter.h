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

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QObject>
#include "dmxvalue.h"
#include "../qontrejour.h"
#include <QList>

class Interpreter
    : public QObject
{

  Q_OBJECT

public :

  explicit Interpreter(QObject *parent = nullptr);

  void recieveData(KeypadButton t_button);

  QList<id> getL_selectedChannels() const
  { return m_L_selectedChannels; }

  void setL_selectedChannels(const QList<id> &t_L_selectedChannels)
  { m_L_selectedChannels = t_L_selectedChannels; }

private :

  void calculateId();
  void calculateFloat();
  void selectId();
  void selectOutput();
  void selectThru();
  void selectPlus();
  void selectMoins();

signals :

  void addSelection(QList<id> t_L_id);
  void removeSelection(QList<id> t_L_id);
  void selectAll();
  void clearSelection();
  void setLevel(dmx t_level);
  void sendError();

public slots :


private :

  int m_valueCount = 0;
  bool m_isValued = false;
  QList<KeypadButton> m_L_digits;
  id m_tempID;
  id m_lastSelectedId;
  float m_tempFloat;
  QList<id> m_L_selectedChannels;

};

#endif // INTERPRETER_H
