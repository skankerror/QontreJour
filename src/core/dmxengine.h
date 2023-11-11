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
#include "dmxscene.h"

/****************************** Ch_Id_Dmx ********************************/

class Ch_Id_Dmx
{

public :

  explicit Ch_Id_Dmx(const id t_ID = NO_ID,
                     const dmx t_level = NULL_DMX)
      : m_ID(t_ID),
      m_level(t_level)
  {}

  virtual ~Ch_Id_Dmx(){}

  bool operator==(const Ch_Id_Dmx t_id_dmx) const
  { return ((t_id_dmx.getid() == m_ID)
            && (t_id_dmx.getLevel() == m_level)); }
  virtual bool isBrother(const Ch_Id_Dmx t_id_dmx) const
  { return (m_ID == t_id_dmx.getid()); }

  // NOTE : use isBrother() before these operators
  bool operator<(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level < t_id_dmx.getLevel()); }
  bool operator>(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level > t_id_dmx.getLevel()); }
  bool operator<=(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level <= t_id_dmx.getLevel()); }
  bool operator>=(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level >= t_id_dmx.getLevel()); }


  id getid() const{ return m_ID; }
  dmx getLevel() const{ return m_level; }

  void setID(const id t_ID){ m_ID = t_ID; }
  void setLevel(const dmx t_level){ m_level = t_level; }

  bool isValid() const{ return (m_ID > NO_ID); }

private :

  id m_ID = NO_ID;
  dmx m_level = NULL_DMX;

};

/****************************** Gr_Id_Dmx ********************************/

class Gr_Id_Dmx
    : public Ch_Id_Dmx
{

public :

  explicit Gr_Id_Dmx(const id t_ID = NO_ID,
                     const dmx t_level = NULL_DMX)
      : Ch_Id_Dmx(t_ID,
                  t_level)
  {}

  ~Gr_Id_Dmx(){}

  bool isBrother(const Ch_Id_Dmx t_id_dmx) const override
  { return false; }
};

/******************************* ChannelData ***********************/

class ChannelData
    : public QObject
{

  Q_OBJECT

public :

  Q_ENUM(ChannelDataFlag)

  explicit ChannelData(id t_id = NO_ID,
                       dmx t_channelGroupLevel = NULL_DMX,
                       dmx t_directChannelLevel = NULL_DMX,
                       overdmx t_directChannelOffset = NULL_DMX_OFFSET,
                       dmx t_sceneLevel = NULL_DMX,
                       dmx t_nextSceneLevel = NULL_DMX,
                       QObject *parent = nullptr)
      : QObject(parent),
      m_channelID(t_id),
      m_channelGroupLevel(t_channelGroupLevel),
      m_directChannelLevel(t_directChannelLevel),
      m_directChannelOffset(t_directChannelOffset),
      m_sceneLevel(t_sceneLevel),
      m_nextSceneLevel(t_nextSceneLevel)
  {}

  ~ChannelData(){}

  dmx getChannelGroupLevel() const{ return m_channelGroupLevel; }
  dmx getDirectChannelLevel() const{ return m_directChannelLevel; }
  overdmx getDirectChannelOffset() const{ return m_directChannelOffset; }
  dmx getSceneLevel() const{ return m_sceneLevel; }
  dmx getNextSceneLevel() const{ return m_nextSceneLevel; }
  dmx getActual_Level() const{ return m_actual_Level; }
  ChannelDataFlag getFlag() const{ return m_flag; }
  id getChannelID() const{ return m_channelID; }
  bool getIsSelected() const{ return m_isSelected; }

  void setChannelGroupLevel(dmx t_channelGroupLevel)
  { m_channelGroupLevel = t_channelGroupLevel; }
  void setDirectChannelLevel(dmx t_directChannelLevel)
  { m_directChannelLevel = t_directChannelLevel; }
  void setDirectChannelOffset(overdmx t_directChannelOffset)
  { m_directChannelOffset = t_directChannelOffset; }
  void setSceneLevel(dmx t_sceneLevel){ m_sceneLevel = t_sceneLevel; }
  void setNextSceneLevel(dmx t_nextSceneLevel)
  { m_nextSceneLevel = t_nextSceneLevel; }
  void setChannelID(id t_channelID){ m_channelID = t_channelID; }
  void setActual_Level(dmx t_actual_Level){ m_actual_Level = t_actual_Level; }
  void setIsSelected(bool t_isSelected)
  {
    m_isSelected = t_isSelected;
    if (!t_isSelected) clearOverdmx();
  }

  void clearOverdmx(){ m_directChannelOffset = NULL_DMX_OFFSET; }
  void update();

private :

  void setFlag(ChannelDataFlag t_flag){ m_flag = t_flag; }

signals :

  void blockChannelSlider(dmx t_actualLevel);

private :

  id m_channelID = NO_ID;
  dmx m_channelGroupLevel = NULL_DMX;
  dmx m_directChannelLevel = NULL_DMX;
  overdmx m_directChannelOffset = NULL_DMX_OFFSET;
  dmx m_sceneLevel = NULL_DMX;
  dmx m_nextSceneLevel = NULL_DMX;
  ChannelDataFlag m_flag = UnknownFlag;
  bool m_isSelected;

  dmx m_actual_Level = NULL_DMX;

};

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

  QList<Sequence *> m_L_seq;
  id m_mainSeqId = 0;
  sceneID_f m_selectedCueId = 0.0f;

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
  void onChannelLevelChangedFromScene(id t_id,
                                      dmx t_level);
  void onChannelLevelChangedFromNextScene(id t_id,
                                          dmx t_level);


private :

  RootValue *m_rootChannel;

  QList<ChannelData *> m_L_channelData;


};

/****************************** OutputEngine *****************************/

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

#endif // DMXENGINE_H

