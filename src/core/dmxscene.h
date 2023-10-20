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
#include "../qontrejour.h"


/****************************** RootScene ****************************/

class DmxScene;

class Sequence :
    public RootValue
{

  Q_OBJECT

public :

  explicit Sequence(ValueType t_type = ValueType::SequenceType,
                    DmxValue *t_parent = nullptr);

  ~Sequence();

  QList<DmxScene *> getL_childScene() const{ return m_L_childScene; }
  DmxScene *getScene(id t_step);
  DmxScene *getScene(sceneID_f t_id);
  int getSize() const{ return m_L_childScene.size() ;}

  void addScene(DmxScene *t_scene);
  void setL_childScene(const QList<DmxScene *> &t_L_childScene)
  { m_L_childScene = t_L_childScene; }

private :

  QList<DmxScene *> m_L_childScene;

};

/****************************** DmxScene *****************************/
class SubScene;

class DmxScene :
    public LeveledValue
{

  Q_OBJECT

public :

  explicit DmxScene(ValueType t_type = ValueType::MainScene,
                    Sequence *t_parent = nullptr);

  virtual ~DmxScene();

  void addSubScene(SubScene *t_subScene);

  // getters
  QString getNotes() const{ return m_notes; }
  time_f getTimeIn() const{ return m_timeIn; }
  time_f getTimeOut() const{ return m_timeOut; }
  time_f getDelayIn() const{ return m_delayIn; }
  time_f getDelayOut() const{ return m_delayOut; }
  id getStepNumber() const{ return LeveledValue::getID(); }
  sceneID_f getSceneID() const{ return m_sceneID; }
  Sequence *getSequence() const{ return m_sequence; }
  QList<SubScene *> getL_subScene() const{ return m_L_subScene; }

  // setters
  void setNotes(const QString &t_notes){ m_notes = t_notes; }
  void setTimeIn(time_f t_timeIn){ m_timeIn = t_timeIn; }
  void setTimeOut(time_f t_timeOut){ m_timeOut = t_timeOut; }
  void setDelayIn(time_f t_delayIn){ m_delayIn = t_delayIn; }
  void setDelayOut(time_f t_delayOut){ m_delayOut = t_delayOut; }
  void setSceneID(sceneID_f t_sceneID){ m_sceneID = t_sceneID; }
  void setStepNumber(id t_stepNumber){ LeveledValue::setID(t_stepNumber); }
  void setSequence(Sequence *t_sequence){ m_sequence = t_sequence; }
  void setL_subScene(const QList<SubScene *> &t_L_subScene)
  { m_L_subScene = t_L_subScene; }

protected :

  sceneID_f m_sceneID;
  QString m_notes;
  time_f m_timeIn;
  time_f m_timeOut;
  time_f m_delayIn;
  time_f m_delayOut;

  Sequence *m_sequence;
  QList<SubScene *> m_L_subScene;

};

/*************************** DmxSubScene *******************************/

class SubScene :
    public DmxScene
{
  Q_OBJECT

public :

  explicit SubScene(ValueType t_type = ValueType::SubSceneType,
                    DmxScene *t_parent = nullptr);

  DmxScene *getParentScene() const
  { return m_parentScene; }

  void setParentScene(DmxScene *t_parentScene)
  { m_parentScene = t_parentScene; }

protected :

  DmxScene *m_parentScene;
};


#endif // DMXSCENE_H
