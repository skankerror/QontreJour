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
  percent calculatePercent();
  bool calculateFloatTime();
  bool calculateStepId();
  void clearCue() { m_selectedCueId = 0.0f; }
  void clearGroup() { m_selectedGroupId = NO_ID; }
  void resetInterpreter()
  { clearValue(); clearGroup(); clearCue(); }

  percent dmxToPercent(dmx t_dLevel);
  dmx percentToDmx(percent t_pLevel);

signals :

  void addChannelSelection(QList<id> t_L_id);
  void removeChannelSelection(QList<id> t_L_id);
  void addOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void removeOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void selectAll();
  void clearChannelSelection();
  void clearOutputSelection();
  void plusPercent();
  void moinsPercent();
  void setTimeIn(time_f t_time);
  void setTimeOut(time_f t_time);
  void setDelayIn(time_f t_time);
  void setDelayOut(time_f t_time);
  void setLevel(dmx t_level);
  void recordNextCue();
  void recordNewCue(sceneID_f t_id);
  void updateCurrentCue();
  void updateCue(sceneID_f t_id);
  void recordGroup(id t_id);
  void gotoCue(sceneID_f t_id);
  void gotoStep(id t_id);
  void deleteCue(sceneID_f t_id);
  void deleteStep(id t_id);
  void deleteGroup(id t_id);
  void sendError();
  void sendError_NoValueSpecified();

public slots :

  void setLastSelectedChannelId(id t_lastSelectedChannelId)
  { m_lastSelectedChannelId = t_lastSelectedChannelId; }

  void setLastSelectedOutputUidId(const Uid_Id &t_lastSelectedOutputUidId)
  { m_lastSelectedOutputUidId = t_lastSelectedOutputUidId; }

private :

  bool m_isValued = false;
  QList<KeypadButton> m_L_digits;
  id m_lastSelectedChannelId = NO_ID;
  Uid_Id m_lastSelectedOutputUidId = NULL_UID_ID;
  id m_selectedGroupId = NO_ID;
  sceneID_f m_selectedCueId = 0.0f;
  id m_stepId = NO_ID;
  time_f m_time = 0.0f;
};
#endif // INTERPRETER_H
