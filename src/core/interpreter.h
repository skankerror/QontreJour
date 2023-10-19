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

  void setLastSelectedChannelId(id t_lastSelectedChannelId)
  { m_lastSelectedChannelId = t_lastSelectedChannelId; }

  void setLastSelectedOutputUidId(const Uid_Id &t_lastSelectedOutputUidId)
  { m_lastSelectedOutputUidId = t_lastSelectedOutputUidId; }

private :

  void clearValue();
  bool calculateChannelId();
  bool calculateOutputUidId();
  dmx calculateDmx();
  bool calculateFloatTime();

signals :

  void addChannelSelection(QList<id> t_L_id);
  void removeChannelSelection(QList<id> t_L_id);
  void addOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void removeOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void selectAll();
  void clearChannelSelection();
  void clearOutputSelection();
  void setLevel(dmx t_level);
  void sendError();
  void sendError_NoValueSpecified();

public slots :


private :

//  int m_valueCount = 0;
  bool m_isValued = false;
  QList<KeypadButton> m_L_digits;
  id m_lastSelectedChannelId = NO_ID;
  Uid_Id m_lastSelectedOutputUidId = NULL_UID_ID;
  float m_tempFloat = 0.0f;

};

#endif // INTERPRETER_H
