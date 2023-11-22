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

#ifndef DMXENGINE_H
#define DMXENGINE_H

#include <QObject>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include "../qontrejour.h"
#include "dmxvalue.h"
#include "channeldataengine.h"

/****************************** ChannelGroupEngine ***********************/

class ChannelGroupEngine :
    public QObject
{

  Q_OBJECT

public :

  explicit ChannelGroupEngine(RootValue *t_rootGroup,
                              QObject *parent = nullptr);

  ~ChannelGroupEngine();

  bool addNewGroup(const DmxChannelGroup *t_newGroup);
  bool addNewGroup(const id t_groupId);
  bool removeGroup(const DmxChannelGroup *t_group);
  bool removeGroup(const id t_groupId);
  bool modifyGroup(const DmxChannelGroup *t_group);
  bool modifyGroup(const id t_groupId);

  DmxChannelGroup *createChannelGroup(QList<DmxChannel *> t_L_channel);
//  DmxChannelGroup *createChannelGroup(QList<id> t_L_channelId);

private :

  void addChannelGroup(id t_groupID,
                       QList<Ch_Id_Dmx> t_L_id_dmx);
  bool addChannel(const id t_groupID,
                  const Ch_Id_Dmx t_id_dmx);
  bool removeChannelGroup(id t_groupID);

signals :

  void channelLevelChangedFromGroup(id t_channelID,
                                    dmx t_level);

public slots :

  void groupLevelChanged(const id t_groupID,
                         const dmx t_level);

private :

  RootValue *m_rootChannelGroup;

  // id : group id , Ch_Id_Dmx : channelId _ stored level
  QMultiMap<id, Ch_Id_Dmx> m_MM_totalGroup;
  // channel Id , Gr_Id_Dmx : higher Group Id _ actual htp level
  QMap<id, Gr_Id_Dmx> m_M_channelLevel;
};

/******************************* GoEngine *****************************/

class GoEngine
    : public QParallelAnimationGroup
{

  Q_OBJECT

public :

  explicit GoEngine(RootValue *t_rootValue,
                    ChannelDataEngine *t_channelDataEngine,
                    QList<Sequence *> t_L_seq,
                    QObject *parent = nullptr);

  void setRootValue(RootValue *t_rootValue){ m_rootValue = t_rootValue; }

  void letsGo(id t_fromSceneStep,
              id t_toSceneStep);
  void letsPause();
  void letsGoBack();

private :

  RootValue *m_rootValue;
  ChannelDataEngine *m_channelDataEngine;
  QList<Sequence *> m_L_seq;

};

/******************************* CueEngine ****************************/

class CueEngine :
    public QObject
{

  Q_OBJECT

public :

  explicit CueEngine(RootValue *t_rootValue,
                     ChannelDataEngine *t_channelDataEngine,
                     QList<Sequence *> t_L_seq,
                     QObject *parent = nullptr);

  QList<Sequence *> getL_seq() const{ return m_L_seq; }

  void setL_seq(const QList<Sequence *> &t_L_seq)
  { m_L_seq = t_L_seq; }
  bool setMainSeqId(id t_mainSeqId);

  Sequence *getMainSeq() const;
  Sequence *getSequence(id t_seqid) const;
  DmxScene *getSelectedScene() const;
  DmxScene *getNextScene() const;
  sceneID_f getSelectedCueId() const{ return m_selectedCueId; }

  bool setSelectedCueId(sceneID_f t_selectedCueId);
  bool setSelectedCueStep(id t_stepId);
  void setSelectedPlus();
  void setSelectedMoins();

  void recordNextCueInMainSeq(DmxScene *t_scene);
  void recordNextCue(DmxScene *t_scene,
                     id t_seqId);
  void recordNewCueInMainSeq(DmxScene *t_scene,
                             sceneID_f t_scId);
  void recordNewCue(id t_seqId,
                    DmxScene *t_scene,
                    sceneID_f t_scId);
  void deleteCueInMainSeq(sceneID_f t_id);
  void deleteCue(id t_seqId,
                 sceneID_f t_id);

  DmxScene *createScene(QList<DmxChannel *> t_L_channel,
                        sceneID_f t_id = 0.0f);
  void updateScene(QList<DmxChannel *> t_L_channel,
                   sceneID_f t_id = 0.0f);

  void goGo();
  void goBack();
  void goPause();

private :

  void setSelectedSceneLevel(dmx t_level);
  void freeL_activeCuesFromSelectedCue();
  void addSceneToL_activeCues(DmxScene *t_scene);
  void newSceneSelected(sceneID_f t_id);

signals :

  void channelLevelChangedFromCue(id t_channelid,
                                  dmx t_level,
                                  CueRole t_role = CueRole::UnknownRole);

public slots :

  void cueLevelChanged(sceneID_f t_sceneid,
                       dmx t_level);

private slots :

  void onSeqChanged(id t_seqId);

private :

  ChannelDataEngine *m_channelDataEngine;

  GoEngine *m_goEngine;

  QList<Sequence *> m_L_seq;
  id m_mainSeqId = 0;
  sceneID_f m_selectedCueId = 0.0f;

  // id : channel id , Ch_Id_Dmx : channelId _ stored level
//  QMultiMap<SeqId_SceneId, id> m_MM_activeCues;

  QList<SeqId_SceneId> m_L_activeCues;
  // channel Id , Sceneid_Dmx : higher scene Id _ actual htp level
  QMap<id, Sceneid_Dmx> m_M_channelMaxLevel;
};

/******************************* ChannelEngine ***********************/

class ChannelEngine
    : public QObject
{

  Q_OBJECT

public :

  explicit ChannelEngine(RootValue *t_rootChannel,
                         ChannelDataEngine *t_channelDataEngine,
                         QObject *parent = nullptr);

  ~ChannelEngine();

  RootValue *getRootChannel() const{ return m_rootChannel; }

  void setRootChannel(RootValue *t_rootChannel)
  { m_rootChannel = t_rootChannel; }
  void setChannelDataEngine(ChannelDataEngine *t_channelDataEngine)
  { m_channelDataEngine = t_channelDataEngine; }

private :

  void update(id t_id);

signals :

  void selectionChanged(QList<id> t_L_id);

public slots :

  void onChannelLevelChangedFromGroup(id t_id,
                                      dmx t_level);
  void onChannelLevelChangedFromDirectChannel(id t_id,
                                              dmx t_level,
                                              overdmx t_offset = NULL_DMX);
  void onSelectedChannelListAtLevel(dmx t_level);
  void onSelectedChannelListPlusLevel();
  void onSelectedChannelListMoinsLevel();
  void onChannelLevelPlusFromDirectChannel(id t_id);
  void onChannelLevelMoinsFromDirectChannel(id t_id);
  void onChannelLevelChangedFromScene(id t_channelid,
                                      dmx t_level,
                                      CueRole t_role = CueRole::NewSelectRole);

  void clearChannelDataSelection();

private :

  RootValue *m_rootChannel;

  ChannelDataEngine *m_channelDataEngine;
};

/****************************** OutputEngine *****************************/

class DmxPatch;

class OutputEngine
    : public QObject
{

  Q_OBJECT

public :

  explicit OutputEngine(QList<RootValue *> t_L_rootOutput,
                        DmxPatch *t_patch,
                        QObject *parent = nullptr);

  ~OutputEngine();

public slots :

  void onChannelLevelChanged(id t_channelId,
                             dmx t_level);
  void onDirectOutputLevelChanged(Uid_Id t_uid_id,
                                  dmx t_level);
  void onDirectOutputLevelPlus(Uid_Id t_uid_id);
  void onDirectOutputLevelMoins(Uid_Id t_uid_id);

private :

  QList<RootValue *> m_L_rootOutput;
  DmxPatch *m_patch;

};

/******************************* DmxEngine ***************************/

class DmxEngine
    : public QObject
{

  Q_OBJECT

public :

  explicit DmxEngine(RootValue *t_rootGroup,
                     RootValue *t_rootChannel,
                     QList<RootValue *> t_L_rootOutput,
                     DmxPatch *t_patch,
                     QList<Sequence *> t_L_seq,
                     QObject *parent = nullptr);

  ~DmxEngine();

  ChannelGroupEngine *getGroupEngine() const{ return m_groupEngine; }
  CueEngine *getCueEngine() const{ return m_cueEngine; }
  ChannelEngine *getChannelEngine() const{ return m_channelEngine; }
  OutputEngine *getOutputEngine() const{ return m_outputEngine; }

  void setMainSeq(id t_id);

  ChannelDataEngine *getChannelDataEngine() const
  { return m_channelDataEngine; }

private :

  QList<DmxChannel *> getSelectedChannels()const;

public slots :

  // connected to interpreter
  void onAddChannelSelection(QList<id> t_L_id);
  void onRemoveChannelSelection(QList<id> t_L_id);
  void onAddOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void onRemoveOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void onSelectAll();
//  void onClearChannelSelection();
  void onClearOutputSelection();
  void onSetLevel(dmx t_level);
  void onSendError();
  void onSendError_NoValueSpecified();
  void onPlusPercent();
  void onMoinsPercent();
  void onSetTimeIn(time_f t_time);
  void onSetTimeOut(time_f t_time);
  void onSetDelayIn(time_f t_time);
  void onSetDelayOut(time_f t_time);
  void onRecordNextCue();
  void onRecordNewCue(sceneID_f t_id);
  void onUpdateCurrentCue();
  void onUpdateCue(sceneID_f t_id);
  void onRecordGroup(id t_id);
//  void onGotoCue(sceneID_f t_id);
//  void onGotoStep(id t_id);
  void onDeleteCue(sceneID_f t_id);
  void onDeleteStep(id t_id);
  void onDeleteGroup(id t_id);

  // TODO : renvoyer le dernier id selectionné à l'interpreter
private :

  ChannelGroupEngine *m_groupEngine;
  CueEngine *m_cueEngine;
  ChannelEngine *m_channelEngine;
  OutputEngine *m_outputEngine;

  ChannelDataEngine *m_channelDataEngine;

  // members for interpreter
  QList<Uid_Id> m_L_outputUid_IdSelection;
  SelectionType m_selType = SelectionType::ChannelSelectionType;

};

/****************************** DmxPatch ******************************/

class DmxPatch
{

public :

  explicit DmxPatch(){}

  ~DmxPatch(){}

  QMultiMap<id, Uid_Id> getMM_patch() const{ return m_MM_patch; }
  QList<Uid_Id> getL_Uid_Id(id t_channelID)
  { return m_MM_patch.values(t_channelID); }

  void setMM_patch(const QMultiMap<id, Uid_Id> &t_MM_patch)
  { m_MM_patch = t_MM_patch; }

  void clearPatch();
  bool clearChannel(const id t_channelID);
  bool addOutputToChannel(const id t_channelID,
                          const Uid_Id t_outputUid_Id);
  void addOutputListToChannel(const id t_channelId,
                              const QList<Uid_Id> t_L_outputUid_Id);
  bool removeOutput(const Uid_Id t_outputUid_Id);
  void removeOutputList(const QList<Uid_Id> t_L_outputUid_Id);
  bool removeOutputFromChannel(const id t_channelID,
                               const Uid_Id t_outputUid_Id);
  void removeOutputListFromChannel(const id t_channelID,
                                   const QList<Uid_Id> t_L_outputUid_Id);

private :

  QMultiMap<id, Uid_Id> m_MM_patch;

};


#endif // DMXENGINE_H

