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


/****************************** Sequence ****************************/

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
  qsizetype getSize() const{ return m_L_childScene.size() ;}
  id getSelectedStepId() const;
  sceneID_f getSelectedSceneId() const{ return m_selectedSceneId; }

  void addScene(DmxScene *t_scene);
  void addScene(DmxScene *t_scene,
                sceneID_f t_id);

  void removeScene(id t_step);
  void removeScene(sceneID_f);

  void setL_childScene(const QList<DmxScene *> &t_L_childScene)
  { m_L_childScene = t_L_childScene; }
  void setSelectedStepId(id t_selectedStepId);
  void setSelectedSceneId(sceneID_f t_selectedSceneId)
  { m_selectedSceneId = t_selectedSceneId; }

signals :

  void seqSignalChanged(id t_id);

public slots :

private:

  // update from t_step till end
  void update(id t_step);

private :

  QList<DmxScene *> m_L_childScene;
  sceneID_f m_selectedSceneId = 0.0f;
//  id m_selectedStepId = 0;
};

/****************************** DmxScene *****************************/

class SubScene;

class DmxScene :
    public DmxChannelGroup
{

  Q_OBJECT

public :

  explicit DmxScene(ValueType t_type = ValueType::MainScene,
                    Sequence *t_parent = nullptr);

  explicit DmxScene(DmxScene &t_scene);

  virtual ~DmxScene();

  bool operator <(const DmxScene &t_scene)const;
  bool operator >(const DmxScene &t_scene)const;
  bool operator <=(const DmxScene &t_scene)const;
  bool operator >=(const DmxScene &t_scene)const;

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
  time_f m_timeIn = 5.0f;
  time_f m_timeOut = 5.0f;
  time_f m_delayIn = 0.0f;
  time_f m_delayOut = 0.0f;

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
