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

#ifndef DMXSCENE_H
#define DMXSCENE_H

#include "dmxvalue.h"


/****************************** RootScene ****************************/

class DmxScene;

class RootScene :
    public DmxValue
{

  Q_OBJECT

public :

  explicit RootScene(DmxValue::ValueType t_type = Sequence,
                     DmxValue *t_parent = nullptr);

  ~RootScene();

private :

  QList<DmxScene *> m_L_childScene;

};

/****************************** DmxScene *****************************/

class DmxScene :
    public DmxValue
{

  Q_OBJECT

public :

  explicit DmxScene(DmxValue::ValueType t_type = MainScene,
                    RootScene *t_parent = nullptr);

  virtual ~DmxScene();

  // getters
  QString getNotes() const{ return m_notes; }
  time_f getTimeIn() const{ return m_timeIn; }
  time_f getTimeOut() const{ return m_timeOut; }
  time_f getDelayIn() const{ return m_delayIn; }
  time_f getDelayOut() const{ return m_delayOut; }
  id getStepNumber() const{ return m_ID; }
  sceneID_f getSceneID() const{ return m_sceneID; }
  RootScene *getSequence() const{ return m_sequence; }

  // setters
  void setNotes(const QString &t_notes){ m_notes = t_notes; }
  void setTimeIn(time_f t_timeIn){ m_timeIn = t_timeIn; }
  void setTimeOut(time_f t_timeOut){ m_timeOut = t_timeOut; }
  void setDelayIn(time_f t_delayIn){ m_delayIn = t_delayIn; }
  void setDelayOut(time_f t_delayOut){ m_delayOut = t_delayOut; }
  void setSceneID(sceneID_f t_sceneID){ m_sceneID = t_sceneID; }
  void setStepNumber(id t_stepNumber){ m_ID = t_stepNumber; }
  void setSequence(RootScene *t_sequence){ m_sequence = t_sequence; }

protected :

  sceneID_f m_sceneID;
  QString m_notes;
  time_f m_timeIn;
  time_f m_timeOut;
  time_f m_delayIn;
  time_f m_delayOut;

  RootScene *m_sequence;

};

/*************************** DmxSubScene *******************************/



#endif // DMXSCENE_H
