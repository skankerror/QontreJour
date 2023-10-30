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

private :

  void clearValue();
  bool calculateChannelId();
  bool calculateOutputUidId();
  bool calculateGroupId();
  bool calculateCueId();
  dmx calculateDmx();
  bool calculateFloatTime();

signals :

  void addChannelSelection(QList<id> t_L_id);
  void removeChannelSelection(QList<id> t_L_id);
  void addOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void removeOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void addGroupSelection(QList<id> t_L_id);
  void removeGroupSelection(QList<id> t_L_id);
  void addCueSelection(QList<sceneID_f> t_L_sceneID);
  void removeCueSelection(QList<sceneID_f> t_L_sceneID);
  void selectAll();
  void clearChannelSelection();
  void clearOutputSelection();
  void clearGroupSelection();
  void clearCueSelection();
  void plusPercent();
  void moinsPercent();
  void setTimeIn(time_f t_time);
  void setTimeOut(time_f t_time);
  void setDelayIn(time_f t_time);
  void setDelayOut(time_f t_time);
  void setLevel(dmx t_level);
  void sendError();
  void sendError_NoValueSpecified();

public slots :

  void setLastSelectedChannelId(id t_lastSelectedChannelId)
  { m_lastSelectedChannelId = t_lastSelectedChannelId; }

  void setLastSelectedOutputUidId(const Uid_Id &t_lastSelectedOutputUidId)
  { m_lastSelectedOutputUidId = t_lastSelectedOutputUidId; }

  void setLastSelectedGroupId(id t_lastSelectedGroupId)
  { m_lastSelectedGroupId = t_lastSelectedGroupId; }

  void setLastSelectedCueId(sceneID_f t_lastSelectedCueId)
  { m_lastSelectedCueId = t_lastSelectedCueId; }

private :

  bool m_isValued = false;
  QList<KeypadButton> m_L_digits;
  id m_lastSelectedChannelId = NO_ID;
  Uid_Id m_lastSelectedOutputUidId = NULL_UID_ID;
  id m_lastSelectedGroupId = NO_ID;
  sceneID_f m_lastSelectedCueId = 0.0f;
  time_f m_time = 0.0f;

};

#endif // INTERPRETER_H
