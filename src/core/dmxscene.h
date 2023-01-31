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


class DmxScene :
//    public DmxChannelGroup
    public DmxValue
{

  Q_OBJECT

public:

  enum SceneType
  {
    RootScene,
    MainScene,
    SubScene,
    Unknow_type
  };
  Q_ENUM(SceneType)

  explicit DmxScene(DmxValue::ValueType t_type = Scene,
                    DmxScene *t_parent = nullptr);

  virtual ~DmxScene();

  // getters
  QString getNotes() const{ return m_notes; }
  double getTimeIn() const{ return m_timeIn; }
  double getTimeOut() const{ return m_timeOut; }
  double getDelayIn() const{ return m_delayIn; }
  double getDelayOut() const{ return m_delayOut; }
  SceneType getType() const{ return m_type; }
  QList<DmxScene *> getL_subScene() const{ return m_L_subScene; }
  DmxScene *getParentSCene() const{ return m_parentSCene; }
  int getStepNumber() const;
  int getSize() const{ return m_L_subScene.size(); }
  DmxScene *getSubscene(int t_number);

  // setters
  void setNotes(const QString &t_notes){ m_notes = t_notes; }
  void setTimeIn(double t_timeIn){ m_timeIn = t_timeIn; }
  void setTimeOut(double t_timeOut){ m_timeOut = t_timeOut; }
  void setDelayIn(double t_delayIn){ m_delayIn = t_delayIn; }
  void setDelayOut(double t_delayOut){ m_delayOut = t_delayOut; }

  bool insertNewScene(int t_position);
  bool insertScene(int t_position,
                   DmxScene *t_scene);
  bool removeScene(int t_position,
                   int t_count = 1);
  bool addScene(DmxScene *t_scene);


public slots :

  void setType(DmxScene::SceneType t_type){ m_type = t_type; }

  void setL_subScene(const QList<DmxScene *> &t_L_subScene)
  { m_L_subScene = t_L_subScene; }

  void setParentSCene(DmxScene *t_parentSCene)
  { m_parentSCene = t_parentSCene; }

  void setStepNumber(int t_stepNumber)
  { m_stepNumber = t_stepNumber; }

signals:

protected :

//  QList<std::pair<DmxChannelGroup *, quint8>> m_L_P_dmxChannelGroup;
  QString m_notes;
  double m_timeIn;
  double m_timeOut;
  double m_delayIn;
  double m_delayOut;
  int m_stepNumber;

  SceneType m_type;

  // for root scene which will contain all a sequence scene
  QList<DmxScene *> m_L_subScene;

  // scene parent :
  // nullptr for rootscene
  // rootscene for mainscene
  // mainscene for subscene
  DmxScene *m_parentSCene;

};

#endif // DMXSCENE_H
