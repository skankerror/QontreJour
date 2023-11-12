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
#include "../qontrejour.h"
#include "dmxvalue.h"

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

/******************************* CueEngine ****************************/

class CueEngine :
    public QObject
{

  Q_OBJECT

public :

  explicit CueEngine(QList<Sequence *> t_L_seq,
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

signals :

  void channelLevelChangedFromCue(sceneID_f t_sceneid,
                                  id t_channelid,
                                  dmx t_level,
                                  CueRole t_role = CueRole::UnknownRole);

//  void channelLevelChangedFromOutingCue(sceneID_f t_sceneid,
//                                        id t_channelid,
//                                        dmx t_level);

//  void channelLevelChangedFromInningCue(sceneID_f t_sceneid,
//                                        id t_channelid,
//                                        dmx t_level);

public slots :

  void cueLevelChanged(sceneID_f t_sceneid,
                       dmx t_level);

private slots :

  void onSeqChanged(id t_seqId);

private :

  QList<Sequence *> m_L_seq;
  id m_mainSeqId = 0;
  sceneID_f m_selectedCueId = 0.0f;

  // channel Id , Gr_Id_Dmx : higher Group Id _ actual htp level
  QMap<id, Gr_Id_Dmx> m_M_channelLevel;

};

/******************************* ChannelEngine ***********************/

class ChannelEngine
    : public QObject
{

  Q_OBJECT

public :

  explicit ChannelEngine(RootValue *t_rootChannel,
                         QObject *parent = nullptr);

  ~ChannelEngine();

  QList<id> selectNonNullChannels();
  void addChannelDataSelection(QList<id> t_L_id);
  void removeChannelDataSelection(QList<id> t_L_id);
  void clearChannelDataSelection();
  QList<id> getSelectedChannelsId();
  RootValue *getRootChannel() const{ return m_rootChannel; }

  QList<ChannelData *> getL_channelData() const
  { return m_L_channelData; }

  void setL_channelData(const QList<ChannelData *> &t_L_channelData)
  { m_L_channelData = t_L_channelData; }

  void setRootChannel(RootValue *t_rootChannel)
  { m_rootChannel = t_rootChannel; }

private :

  void createDatas(int t_channelCount);
  void update(id t_id);

signals :

  void selectionChanged(QList<id> t_L_id);


public slots :

  void onChannelLevelChangedFromGroup(id t_id,
                                      dmx t_level);
  void onChannelLevelChangedFromDirectChannel(id t_id,
                                              dmx t_level,
                                              overdmx t_offset = NULL_DMX);
  void onChannelLevelPlusFromDirectChannel(id t_id);
  void onChannelLevelMoinsFromDirectChannel(id t_id);
  void onChannelLevelChangedFromScene(sceneID_f t_sceneid,
                                      id t_channelid,
                                      dmx t_level,
                                      CueRole t_role);

private :

  RootValue *m_rootChannel;

  QList<ChannelData *> m_L_channelData;
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

private :

  QList<DmxChannel *> getSelectedChannels()const;

signals :

  void channelSelectionChanged();

public slots :

  // connected to interpreter
  void onAddChannelSelection(QList<id> t_L_id);
  void onRemoveChannelSelection(QList<id> t_L_id);
  void onAddOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void onRemoveOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void onSelectAll();
  void onClearChannelSelection();
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
  void onGotoCue(sceneID_f t_id);
  void onGotoStep(id t_id);
  void onDeleteCue(sceneID_f t_id);
  void onDeleteStep(id t_id);
  void onDeleteGroup(id t_id);

  // TODO : renvoyer le dernier id selectionné à l'interpreter
private :

  ChannelGroupEngine *m_groupEngine;
  CueEngine *m_cueEngine;
  ChannelEngine *m_channelEngine;
  OutputEngine *m_outputEngine;

  // members for interpreter
  QList<id> m_L_channelsIdSelection;
  QList<Uid_Id> m_L_outputUid_IdSelection;
  SelectionType m_selType = SelectionType::UnknownSelectionType;

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

