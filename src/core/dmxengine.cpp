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

#include "dmxengine.h"
#include <QDebug>

/****************************** ChannelGroupEngine ***********************/

#define GET_CHANNEL_GROUP(x) static_cast<DmxChannelGroup*>(m_rootChannelGroup->getChildValue(x))

ChannelGroupEngine::ChannelGroupEngine(RootValue *t_rootGroup,
                                       QObject *parent):
  QObject(parent),
  m_rootChannelGroup(t_rootGroup)
{}

ChannelGroupEngine::~ChannelGroupEngine()
{}

bool ChannelGroupEngine::addNewGroup(const DmxChannelGroup *t_newGroup)
{
  auto H_controledChannel_storedLevel
      = t_newGroup->getH_controledChannel_storedLevel();

  QHash<DmxChannel *, dmx>::const_iterator i
      = H_controledChannel_storedLevel.constBegin();

  while (i != H_controledChannel_storedLevel.constEnd())
  {
    addChannel(t_newGroup->getid(),
               Ch_Id_Dmx(i.key()->getid(),
                         i.value()));
    ++i;
  }

  connect(t_newGroup,
          SIGNAL(levelChanged(id,dmx)),
          this,
          SLOT(groupLevelChanged(id,dmx)));

  return true;
}

bool ChannelGroupEngine::addNewGroup(const id t_groupId)
{
  return addNewGroup(GET_CHANNEL_GROUP(t_groupId));
}

bool ChannelGroupEngine::removeGroup(const DmxChannelGroup *t_group)
{
  return removeChannelGroup(t_group->getid());
  disconnect(t_group,
             SIGNAL(levelChanged(id,dmx)),
             this,
             SLOT(groupLevelChanged(id,dmx)));
}

bool ChannelGroupEngine::removeGroup(const id t_groupId)
{
  return removeChannelGroup(t_groupId);
}

bool ChannelGroupEngine::modifyGroup(const DmxChannelGroup *t_group)
{
  return (removeGroup(t_group) && addNewGroup(t_group));
}

bool ChannelGroupEngine::modifyGroup(const id t_groupID)
{
  return modifyGroup(GET_CHANNEL_GROUP(t_groupID));
}

DmxChannelGroup *ChannelGroupEngine::createChannelGroup(QList<DmxChannel *> t_L_channel)
{
  auto newGroup = new DmxChannelGroup(ValueType::ChannelGroup);
  newGroup->setid(m_rootChannelGroup->getL_childValueSize());
  auto H_controledChannel_storedLevel = QHash<DmxChannel *, dmx>();
  for (const auto item
       : std::as_const(t_L_channel))
  {
    H_controledChannel_storedLevel.insert(item,
                                          item->getLevel());
  }
  newGroup->setH_controledChannel_storedLevel(H_controledChannel_storedLevel);
  m_rootChannelGroup->addChildValue(newGroup);

  addNewGroup(newGroup);

  return newGroup;
}

void ChannelGroupEngine::addChannelGroup(id t_groupID,
                                         QList<Ch_Id_Dmx> t_L_id_dmx)
{
  for (const auto &item
       : std::as_const(t_L_id_dmx))
  {
    addChannel(t_groupID,
               item);
  }
}

bool ChannelGroupEngine::removeChannelGroup(id t_groupID)
{
  // avant il faut nettoyer m_M_channelLevel
  auto L_Ch_Id_Dmx = m_MM_totalGroup.values(t_groupID);
  for (const auto &item
       : std::as_const(L_Ch_Id_Dmx))
  {
    m_M_channelLevel.remove(item.getid());
  }
  return m_MM_totalGroup.remove(t_groupID);
}

bool ChannelGroupEngine::addChannel(const id t_groupID,
                                    const Ch_Id_Dmx t_id_dmx)
{
  if (!(t_id_dmx.isValid()))
  {
    qWarning() << "invalid channel ID GlobalChannelGroup::addChannel";
    return false;
  }
  auto L_id_dmx = m_MM_totalGroup.values(t_groupID);
  if (L_id_dmx.contains(t_id_dmx))
  {
    qWarning() << "channel already stored GlobalChannelGroup::addChannel";
    return false;
  }

  m_MM_totalGroup.insert(t_groupID,
                         t_id_dmx);

  // we insert in channel level map with value 0
  // NOTE : must update level after creation
  if (!(m_M_channelLevel.contains(t_id_dmx.getid())))
  {
    m_M_channelLevel.insert(t_id_dmx.getid(),
                            NULL_GR_ID_DMX);
  }
  return true;
}

void ChannelGroupEngine::groupLevelChanged(const id t_groupID,
                                           const dmx t_level)
{
  // on chope les values de la key t_groupID
  auto L_Ch_Id_Dmx = m_MM_totalGroup.values(t_groupID);
  for (const auto &item
       : std::as_const(L_Ch_Id_Dmx))
  {
    auto actualCh_Id_Dmx = m_M_channelLevel.value(item.getid());
    auto newLevel = (dmx)(((float)(t_level)/255.0f)
                          * item.getLevel());

    if (newLevel > actualCh_Id_Dmx.getLevel()
        || t_groupID == actualCh_Id_Dmx.getid())
    {
      m_M_channelLevel.insert(item.getid(),
                              Gr_Id_Dmx(t_groupID,
                                        newLevel));
      // emit to channel engine
      emit channelLevelChangedFromGroup(item.getid(),
                                        newLevel);
    }
  }
}

/******************************* CueEngine ***************************/

CueEngine::CueEngine(QList<Sequence *> t_L_seq,
                     QObject *parent)
    : m_L_seq(t_L_seq),
    QObject(parent)
{
  for (const auto &item
       : std::as_const(t_L_seq))
  {
    connect(item,
            &Sequence::seqSignalChanged,
            this,
            &CueEngine::onSeqChanged);
  }
  setMainSeqId(0);
}

bool CueEngine::setMainSeqId(id t_mainSeqId)
{
  if (t_mainSeqId < m_L_seq.size()
      && t_mainSeqId > -1
      /*&& t_mainSeqId != m_mainSeqId*/)
  {
    // disconnect cue from seq
    auto seq = getMainSeq();
    if (seq)
    {
      auto L_scene = seq->getL_childScene();
      for (const auto &item
           : std::as_const(L_scene))
      {
        disconnect(item,
                   &DmxScene::sceneLevelChanged,
                   this,
                   &CueEngine::cueLevelChanged);
      }
      setSelectedSceneLevel(NULL_DMX);
      freeL_activeCuesFromSelectedCue();
    }
    // connect cues of new seq
    m_mainSeqId = t_mainSeqId;
    seq = getMainSeq();
    if (seq)
    {
      auto L_scene = seq->getL_childScene();
      for (const auto &item
           : std::as_const(L_scene))
      {
        connect(item,
                &DmxScene::sceneLevelChanged,
                this,
                &CueEngine::cueLevelChanged);
      }
      m_L_activeCues.clear();
      // TODO : clear direct channel
      auto scene = getSelectedScene();
      addSceneToL_activeCues(scene);
      setSelectedSceneLevel(MAX_DMX);
      return true;
    }
    qDebug() << "CueEngine::setMainSeqId preblem";
    return false;
  }
  else
  {
    qDebug() << "CueEngine::setMainSeqId preblem";
    return false;
  }
}

Sequence *CueEngine::getMainSeq() const
{
  if (m_mainSeqId < m_L_seq.size())
    return m_L_seq.at(m_mainSeqId);
  return nullptr;
}

Sequence *CueEngine::getSequence(id t_seqid) const
{
  if (t_seqid < m_L_seq.size())
    return m_L_seq.at(t_seqid);
  return nullptr;
}

DmxScene *CueEngine::getSelectedScene() const
{
  auto seq = getMainSeq();
  if (seq)
    return seq->getScene(m_selectedCueId);
  return nullptr;
}

DmxScene *CueEngine::getNextScene() const
{
  auto seq = getMainSeq();
  if (seq)
  {
    auto scene = seq->getScene(m_selectedCueId);
    if (scene)
    {
      id step = scene->getStepNumber() + 1;
      auto nextScene = seq->getScene(step);
      return nextScene;
    }
  }
  return nullptr;
}

bool CueEngine::setSelectedCueId(sceneID_f t_selectedCueId)
{
  auto seq = getMainSeq();
  if (!seq)
  {
    qDebug() << "CueEngine::setSelectedCueId problem";
    return false;
  }
  auto scene = seq->getScene(t_selectedCueId);
  if (scene)
  {
    setSelectedSceneLevel(NULL_DMX);
    freeL_activeCuesFromSelectedCue();
    m_selectedCueId = t_selectedCueId;
    addSceneToL_activeCues(scene);
    setSelectedSceneLevel(MAX_DMX);
    // update seq for model
    seq->setSelectedSceneId(t_selectedCueId);
    return true;
  }
  else
  {
    qDebug() << "CueEngine::setSelectedCueId problem_";
    return false;
  }
}

bool CueEngine::setSelectedCueStep(id t_stepId)
{
  auto seq = getMainSeq();
  if (!seq)
  {
    qDebug() << "CueEngine::setSelectedCueStep problem";
    return false;
  }
  auto scene = seq->getScene(t_stepId);
  if (scene)
  {
    setSelectedSceneLevel(NULL_DMX);
    freeL_activeCuesFromSelectedCue();
    m_selectedCueId = scene->getSceneID();
    addSceneToL_activeCues(scene);
    setSelectedSceneLevel(MAX_DMX);
    // update seq for model
    seq->setSelectedStepId(t_stepId);
    return true;
  }
  else
  {
    qDebug() << "CueEngine::setSelectedCueStep problem_";
    return false;
  }
}

void CueEngine::setSelectedPlus()
{
  auto seq = getMainSeq();
  if (seq)
  {
    id stepId = seq->getSelectedStepId() + 1;
    seq->setSelectedStepId(stepId);
    setSelectedCueStep(stepId);
  }
}

void CueEngine::setSelectedMoins()
{
  auto seq = getMainSeq();
  if (seq)
  {
    id stepId = seq->getSelectedStepId() -1;
    seq->setSelectedStepId(stepId);
    setSelectedCueStep(stepId);
  }
}

void CueEngine::recordNextCueInMainSeq(DmxScene *t_scene)
{
  auto seq = getMainSeq();
  connect(t_scene,
          &DmxScene::sceneLevelChanged,
          this,
          &CueEngine::cueLevelChanged);
  seq->addScene(t_scene);
  setSelectedSceneLevel(NULL_DMX);
  freeL_activeCuesFromSelectedCue();
  setSelectedCueId(t_scene->getSceneID());
  setSelectedSceneLevel(MAX_DMX);
  addSceneToL_activeCues(t_scene);
}

void CueEngine::recordNextCue(DmxScene *t_scene,
                              id t_seqId)
{
  auto seq = m_L_seq.at(t_seqId);
  connect(t_scene,
          &DmxScene::sceneLevelChanged,
          this,
          &CueEngine::cueLevelChanged);
  // TODO : faut voir ça, pls sequences qui agissent...
  seq->addScene(t_scene);
  setSelectedSceneLevel(NULL_DMX);
  freeL_activeCuesFromSelectedCue();
  setSelectedCueId(t_scene->getSceneID());
  setSelectedSceneLevel(MAX_DMX);
  addSceneToL_activeCues(t_scene);
}

void CueEngine::recordNewCueInMainSeq(DmxScene *t_scene,
                                      sceneID_f t_scId)
{
  auto seq = getMainSeq();
  connect(t_scene,
          &DmxScene::sceneLevelChanged,
          this,
          &CueEngine::cueLevelChanged);
  seq->addScene(t_scene,
                t_scId);
  setSelectedSceneLevel(NULL_DMX);
  freeL_activeCuesFromSelectedCue();
  setSelectedCueId(t_scene->getSceneID());
  setSelectedSceneLevel(MAX_DMX);
  addSceneToL_activeCues(t_scene);
}

void CueEngine::recordNewCue(id t_seqId,
                             DmxScene *t_scene,
                             sceneID_f t_scId)
{
  auto seq = m_L_seq.at(t_seqId);
  connect(t_scene,
          &DmxScene::sceneLevelChanged,
          this,
          &CueEngine::cueLevelChanged);
  seq->addScene(t_scene,
                t_scId);
  setSelectedSceneLevel(NULL_DMX);
  freeL_activeCuesFromSelectedCue();
  setSelectedCueId(t_scene->getSceneID());
  setSelectedSceneLevel(MAX_DMX);
  addSceneToL_activeCues(t_scene);
}

void CueEngine::deleteCueInMainSeq(sceneID_f t_id)
{

}

void CueEngine::deleteCue(id t_seqId,
                          sceneID_f t_id)
{

}

DmxScene *CueEngine::createScene(QList<DmxChannel *> t_L_channel,
                                 sceneID_f t_id)
{
  auto newScene = new DmxScene(ValueType::MainScene);
  auto H_controledChannel_storedLevel = QHash<DmxChannel *, dmx>();
  for (const auto item
       : std::as_const(t_L_channel))
  {
    H_controledChannel_storedLevel.insert(item,
                                          item->getLevel());
  }
  newScene->setH_controledChannel_storedLevel(H_controledChannel_storedLevel);
  newScene->setSceneID(t_id);
  return newScene;
}

void CueEngine::updateScene(QList<DmxChannel *> t_L_channel,
                            sceneID_f t_id)
{

}

void CueEngine::goGo()
{

}

void CueEngine::goBack()
{

}

void CueEngine::goPause()
{

}

void CueEngine::setSelectedSceneLevel(dmx t_level)
{
  auto scene = getSelectedScene();
  if (scene) scene->setLevel(t_level);

}

void CueEngine::freeL_activeCuesFromSelectedCue()
{
  SeqId_SceneId id_sceneid(m_mainSeqId,
                           m_selectedCueId);
  auto index = m_L_activeCues.indexOf(id_sceneid);
  if (index != -1)
  {
    m_L_activeCues.remove(index);
    m_L_activeCues.squeeze();
  }
}

void CueEngine::addSceneToL_activeCues(DmxScene *t_scene)
{
  id seqId = t_scene->getSequence()->getid();
  sceneID_f sceneid= t_scene->getSceneID();
  SeqId_SceneId id_sceneid(seqId,
                           sceneid);
  auto index = m_L_activeCues.indexOf(id_sceneid);
  if (index = -1)
  {
    m_L_activeCues.append(id_sceneid);
  }
}

void CueEngine::newSceneSelected(sceneID_f t_id)
{

}

void CueEngine::cueLevelChanged(sceneID_f t_sceneid,
                                dmx t_level)
{
  // URGENT : vérifier si elle est ds la L
  auto scene = getMainSeq()->getScene(t_sceneid);
  if (scene)
  {
    auto H_controledChannel_storedLevel = scene->getH_controledChannel_storedLevel();
    if (!H_controledChannel_storedLevel.size()) return;
    for (auto i = H_controledChannel_storedLevel.cbegin(),
         end = H_controledChannel_storedLevel.cend();
         i != end;
         ++i)
    {
      auto channel = i.key();
      auto level = i.value();
      auto newLevel = (dmx)(((float)(t_level)/255.0f)
                             * level);

      // URGENT : update la map puis éventuellement ça :
      emit channelLevelChangedFromCue(channel->getid(),
                                      newLevel);
    }
  }
}

void CueEngine::onSeqChanged(id t_seqId)
{
  auto seq = getSequence(t_seqId);
  if (seq)
  {
    if (t_seqId == m_mainSeqId)
      m_selectedCueId = seq->getSelectedSceneId();
  }
}

/******************************* ChannelEngine ***********************/

#define GET_CHANNEL(x) static_cast<DmxChannel*>(m_rootChannel->getChildValue(x))

ChannelEngine::ChannelEngine(RootValue *t_rootChannel,
                             QObject *parent)
  : QObject(parent),
    m_rootChannel(t_rootChannel)
{
  createDatas(m_rootChannel->getL_childValueSize());
}

ChannelEngine::~ChannelEngine()
{
  for (const auto &item
       : std::as_const(m_L_channelData))
  {
    item->deleteLater();
//    item = nullptr;
  }
  m_L_channelData.clear();
  m_L_channelData.squeeze();
}

QList<id> ChannelEngine::selectNonNullChannels()
{
  QList<id> L_selectedId;
  for (const auto &item
       : std::as_const(m_L_channelData))
  {
    dmx level = item->getActual_Level();
    ChannelDataFlag flag = item->getFlag();
    if (level > 0
        && (flag == ChannelDataFlag::SelectedSceneFlag
            || flag == ChannelDataFlag::DirectChannelFlag
            || flag == ChannelDataFlag::ChannelGroupFlag))
    {
      L_selectedId.append(item->getChannelID());
      item->setIsSelected(true);
    }
  }
  emit selectionChanged(getSelectedChannelsId());
//  qDebug() << L_selectedId;
  return L_selectedId;
}

void ChannelEngine::addChannelDataSelection(QList<id> t_L_id)
{
  for (qsizetype i = 0;
       i < t_L_id.size();
       i++)
  {
    auto selectId = t_L_id.at(i);
    ChannelData *channelData = m_L_channelData.at(selectId);
    channelData->setIsSelected(true);
  }
  emit selectionChanged(getSelectedChannelsId());
}

void ChannelEngine::removeChannelDataSelection(QList<id> t_L_id)
{
  for (qsizetype i = 0;
       i < t_L_id.size();
       i++)
  {
    auto selectId = t_L_id.at(i);
    ChannelData *channelData = m_L_channelData.at(selectId);
    channelData->setIsSelected(false);
  }
  emit selectionChanged(getSelectedChannelsId());
}

void ChannelEngine::clearChannelDataSelection()
{
  for (const auto &item
       : std::as_const(m_L_channelData))
  {
    item->setIsSelected(false);
  }
  emit selectionChanged(QList<id>());
}

QList<id> ChannelEngine::getSelectedChannelsId()
{
  QList<id> L_selectedId;
  for (const auto &item
       : std::as_const(m_L_channelData))
  {
    if (item->getIsSelected())
      L_selectedId.append(item->getChannelID());
  }
  return L_selectedId;
}

void ChannelEngine::createDatas(int t_channelCount)
{
  for (int i = 0;
       i < t_channelCount;
       i++)
  {
    auto channelData = new ChannelData(i);
    m_L_channelData.append(channelData);
  }
}

void ChannelEngine::addToL_directChannelIds(id t_id)
{
  auto index = m_L_directChannelId.indexOf(t_id);
  if (index = -1)
  {
    m_L_directChannelId.append(t_id);
  }
}

void ChannelEngine::removeFromL_directChannelIds(id t_id)
{
  auto index = m_L_directChannelId.indexOf(t_id);
  if (index != -1)
  {
    m_L_directChannelId.remove(index);
  }
}

void ChannelEngine::clearL_directChannelIds()
{
  for (qsizetype i = 0;
       i < m_L_directChannelId.size();
       i++)
  {
    auto channelData = m_L_channelData.at(i);
    channelData->clearDirectChannel();
  }
  m_L_directChannelId.clear();
  m_L_directChannelId.squeeze();
}

void ChannelEngine::update(id t_id)
{
  auto channelData = m_L_channelData.at(t_id);
  auto channel = GET_CHANNEL(t_id);
  channelData->update();
  channel->setLevel(channelData->getActual_Level());
  channel->setChannelDataFlag(channelData->getFlag());
}

void ChannelEngine::onChannelLevelChangedFromGroup(id t_id,
                                                   dmx t_level)
{
  auto channelData = m_L_channelData.at(t_id);
  channelData->setChannelGroupLevel(t_level);
  update(t_id);
}

void ChannelEngine::onChannelLevelChangedFromDirectChannel(id t_id,
                                                           dmx t_level,
                                                           overdmx t_offset)
{
  auto channelData = m_L_channelData.at(t_id);
  channelData->setDirectChannelLevel(t_level);
  channelData->setDirectChannelOffset(t_offset);
  addToL_directChannelIds(t_id);
  channelData->setIsDirectChannel(true);
  update(t_id);
}

void ChannelEngine::onChannelLevelPlusFromDirectChannel(id t_id)
{
  auto channelData = m_L_channelData.at(t_id);
  auto level = channelData->getDirectChannelLevel();
  if (level > MAX_DMX - PLUS_DMX)
  {
    auto offset = channelData->getDirectChannelOffset()
                  + level
                  - MAX_DMX
                  - PLUS_DMX;
    channelData->setDirectChannelOffset(offset);
  }
  channelData->setDirectChannelLevel(level + PLUS_DMX);
  addToL_directChannelIds(t_id);
  channelData->setIsDirectChannel(true);
  update(t_id);
}

void ChannelEngine::onChannelLevelMoinsFromDirectChannel(id t_id)
{
  auto channelData = m_L_channelData.at(t_id);
  auto level = channelData->getDirectChannelLevel();
  if (level - MOINS_DMX < 0)
  {
    auto offset = channelData->getDirectChannelOffset()
                  + level
                  - MOINS_DMX;
    channelData->setDirectChannelOffset(offset);
  }
  channelData->setDirectChannelLevel(level - MOINS_DMX);
  addToL_directChannelIds(t_id);
  channelData->setIsDirectChannel(true);
  update(t_id);
}

void ChannelEngine::onChannelLevelChangedFromScene(id t_channelid,
                                                   dmx t_level,
                                                   CueRole t_role /*= CueRole::NewSelectRole*/)
{
  Q_UNUSED(t_role) // NOTE : pour plus tard
  auto channelData = m_L_channelData.at(t_channelid);
  channelData->clearDirectChannel();
  removeFromL_directChannelIds(t_channelid);
  channelData->setSceneLevel(t_level);
  update(t_channelid);
}

/******************************** OutputEngine *************************/

OutputEngine::OutputEngine(QList<RootValue *> t_L_rootOutput,
                           DmxPatch *t_patch,
                           QObject *parent)
  : QObject(parent),
    m_L_rootOutput(t_L_rootOutput),
    m_patch(t_patch)
{}

OutputEngine::~OutputEngine()
{}

void OutputEngine::onChannelLevelChanged(id t_channelId,
                                         dmx t_level)
{
//  qDebug() << "channel id" << t_channelId << "level" << t_level;
  auto L_Uid_Id = m_patch->getL_Uid_Id(t_channelId);
  for (const auto &i
       : std::as_const(L_Uid_Id))
  {
    auto rootOutput = m_L_rootOutput.at(i.getUniverseID());
    auto output = rootOutput->getChildValue(i.getOutputID());
    output->setLevel(t_level);
  }
}

void OutputEngine::onDirectOutputLevelChanged(Uid_Id t_uid_id,
                                              dmx t_level)
{
    auto rootOutput = m_L_rootOutput.at(t_uid_id.getUniverseID());
    auto output = rootOutput->getChildValue(t_uid_id.getOutputID());
    output->setLevel(t_level);

}

void OutputEngine::onDirectOutputLevelPlus(Uid_Id t_uid_id)
{
    auto rootOutput = m_L_rootOutput.at(t_uid_id.getUniverseID());
    auto output = rootOutput->getChildValue(t_uid_id.getOutputID());
    int level = output->getLevel() + PLUS_DMX;
    if (level > MAX_DMX) level = MAX_DMX;
    output->setLevel(level);
}

void OutputEngine::onDirectOutputLevelMoins(Uid_Id t_uid_id)
{
    auto rootOutput = m_L_rootOutput.at(t_uid_id.getUniverseID());
    auto output = rootOutput->getChildValue(t_uid_id.getOutputID());
    int level = output->getLevel() - MOINS_DMX;
    if (level < NULL_DMX) level = NULL_DMX;
    output->setLevel(level);
}

/******************************* DmxEngine ***************************/

DmxEngine::DmxEngine(RootValue *t_rootGroup,
                     RootValue *t_rootChannel,
                     QList<RootValue *> t_L_rootOutput,
                     DmxPatch *t_patch,
                     QList<Sequence *> t_L_seq,
                     QObject *parent)
    : QObject(parent),
    m_groupEngine(new ChannelGroupEngine(t_rootGroup,
                                         this)),
    m_cueEngine(new CueEngine(t_L_seq,
                              this)),
    m_channelEngine(new ChannelEngine(t_rootChannel,
                                      this)),
    m_outputEngine(new OutputEngine(t_L_rootOutput,
                                    t_patch,
                                    this))
{
  connect(m_groupEngine,
          SIGNAL(channelLevelChangedFromGroup(id,dmx)),
          m_channelEngine,
          SLOT(onChannelLevelChangedFromGroup(id,dmx)));

  connect(m_cueEngine,
          &CueEngine::channelLevelChangedFromCue,
          m_channelEngine,
          &ChannelEngine::onChannelLevelChangedFromScene);

  // connect all channels to OutputEngine
  auto L_channel = t_rootChannel->getL_childValue();
  for (const auto &item
       : std::as_const(L_channel))
  {
    connect(item,
            SIGNAL(levelChanged(id,dmx)),
            m_outputEngine,
            SLOT(onChannelLevelChanged(id,dmx)));
  }
}

DmxEngine::~DmxEngine()
{
  m_groupEngine->deleteLater();
  m_cueEngine->deleteLater();
  m_channelEngine->deleteLater();
  m_outputEngine->deleteLater();
}

void DmxEngine::setMainSeq(id t_id)
{
  m_cueEngine->setMainSeqId(t_id);
}

QList<DmxChannel *> DmxEngine::getSelectedChannels() const
{
  auto rootChannel = m_channelEngine->getRootChannel();
  auto L_channel = QList<DmxChannel *>();
  for (qsizetype i = 0;
       i < m_L_channelsIdSelection.size();
       i++)
  {
    auto channel
        = qobject_cast<DmxChannel *>(rootChannel
                                         ->getChildValue(m_L_channelsIdSelection
                                                             .at(i)));
    L_channel.append(channel);
  }
  return L_channel;
}

void DmxEngine::onAddChannelSelection(QList<id> t_L_id)
{
  for (qsizetype i = 0;
       i < t_L_id.size();
       i++)
  {
    id channelId = t_L_id.at(i);
    if (m_L_channelsIdSelection.indexOf(channelId) == -1)
    {
      m_L_channelsIdSelection.append(channelId);
//      emit ChannelSelectionChanged();
    }
  }
  m_selType = SelectionType::ChannelSelectionType;
  // TODO : c'est chiant, il faut updater les channeldata qui sont dans le channel engine...

}

void DmxEngine::onRemoveChannelSelection(QList<id> t_L_id)
{
  for (qsizetype i = 0;
       i < t_L_id.size();
       i++)
  {
    id channelId = t_L_id.at(i);
    int index = m_L_channelsIdSelection.indexOf(channelId);
    if (index != -1)
    {
      m_L_channelsIdSelection.remove(index);
//      emit ChannelSelectionChanged();
    }
  }
}

void DmxEngine::onAddOutputSelection(QList<Uid_Id> t_L_Uid_Id)
{
  for (qsizetype i = 0;
       i < t_L_Uid_Id.size();
       i++)
  {
    Uid_Id outputUid_Id = t_L_Uid_Id.at(i);
    if (m_L_outputUid_IdSelection.indexOf(outputUid_Id) == -1)
    {
      m_L_outputUid_IdSelection.append(outputUid_Id);
    }
  }
  m_selType = SelectionType::OutputSelectionType;
}

void DmxEngine::onRemoveOutputSelection(QList<Uid_Id> t_L_Uid_Id)
{
  for (qsizetype i = 0;
       i < t_L_Uid_Id.size();
       i++)
  {
    Uid_Id outputUid_Id = t_L_Uid_Id.at(i);
    int index = m_L_outputUid_IdSelection.indexOf(outputUid_Id);
    if (index != -1)
    {
      m_L_outputUid_IdSelection.remove(index);
    }
  }
}

void DmxEngine::onSelectAll()
{
  m_L_channelsIdSelection = m_channelEngine->selectNonNullChannels();
  m_selType = SelectionType::ChannelSelectionType;
//  emit channelSelectionChanged();
}

void DmxEngine::onClearChannelSelection()
{
  m_L_channelsIdSelection.clear();
  m_L_channelsIdSelection.squeeze();
  emit channelSelectionChanged();
}

void DmxEngine::onClearOutputSelection()
{

  m_L_outputUid_IdSelection.clear();
  m_L_outputUid_IdSelection.squeeze();
}

void DmxEngine::onSetLevel(dmx t_level)
{
  // we have a problem
  if (m_selType == SelectionType::UnknownSelectionType)
  {
    qDebug() << "error unknown selection type";
    return;
  }
  // that's channel
  if (m_selType == SelectionType::ChannelSelectionType)
  {
    for (qsizetype i = 0;
         i < m_L_channelsIdSelection.size();
         i++)
    {
      m_channelEngine
          ->onChannelLevelChangedFromDirectChannel(m_L_channelsIdSelection.at(i),
                                                   t_level);
    }
    return;
  }
  // that's output
  if (m_selType == SelectionType::ChannelSelectionType)
  {
    for (qsizetype i = 0;
         i < m_L_outputUid_IdSelection.size();
         i++)
    {
      m_outputEngine
          ->onDirectOutputLevelChanged(m_L_outputUid_IdSelection.at(i),
                                       t_level);
    }
    return;
  }
  return;
}

void DmxEngine::onSendError()
{
  qDebug() << "error from interpreter";
}

void DmxEngine::onSendError_NoValueSpecified()
{
  qDebug() << "error from interpreter no value specified";
}

void DmxEngine::onPlusPercent()
{
  // we have a problem
  if (m_selType == SelectionType::UnknownSelectionType)
  {
    qDebug() << "error unknown selection type";
    return;
  }
  // that's channel
  if (m_selType == SelectionType::ChannelSelectionType)
  {
    for (qsizetype i = 0;
         i < m_L_channelsIdSelection.size();
         i++)
    {
      m_channelEngine
          ->onChannelLevelPlusFromDirectChannel(m_L_channelsIdSelection.at(i));
    }
    return;
  }
  // that's output
  if (m_selType == SelectionType::ChannelSelectionType)
  {
    for (qsizetype i = 0;
         i < m_L_outputUid_IdSelection.size();
         i++)
    {
      m_outputEngine
          ->onDirectOutputLevelPlus(m_L_outputUid_IdSelection.at(i));
    }
    return;
  }
  return;
}

void DmxEngine::onMoinsPercent()
{
  // we have a problem
  if (m_selType == SelectionType::UnknownSelectionType)
  {
    qDebug() << "error unknown selection type";
    return;
  }
  // that's channel
  if (m_selType == SelectionType::ChannelSelectionType)
  {
    for (qsizetype i = 0;
         i < m_L_channelsIdSelection.size();
         i++)
    {
      m_channelEngine
          ->onChannelLevelMoinsFromDirectChannel(m_L_channelsIdSelection.at(i));
    }
    return;
  }
  // that's output
  if (m_selType == SelectionType::ChannelSelectionType)
  {
    for (qsizetype i = 0;
         i < m_L_outputUid_IdSelection.size();
         i++)
    {
      m_outputEngine
          ->onDirectOutputLevelMoins(m_L_outputUid_IdSelection.at(i));
    }
    return;
  }
  return;
}

void DmxEngine::onSetTimeIn(time_f t_time)
{
  auto scene = m_cueEngine->getNextScene();
  if (scene)
    scene->setTimeIn(t_time);
}

void DmxEngine::onSetTimeOut(time_f t_time)
{
  auto scene = m_cueEngine->getNextScene();
  if (scene)
    scene->setTimeOut(t_time);
}

void DmxEngine::onSetDelayIn(time_f t_time)
{
  auto scene = m_cueEngine->getNextScene();
  if (scene)
    scene->setDelayIn(t_time);
}

void DmxEngine::onSetDelayOut(time_f t_time)
{
  auto scene = m_cueEngine->getNextScene();
  if (scene)
    scene->setDelayOut(t_time);
}

void DmxEngine::onRecordNextCue()
{
  if (m_selType != SelectionType::ChannelSelectionType)
  {
    qDebug() << " no channels selected"; // NOTE: black scene ?
//    return;
  }
  auto L_channel = getSelectedChannels();
  m_cueEngine
      ->recordNextCueInMainSeq(m_cueEngine
                                  ->createScene(L_channel));
}

void DmxEngine::onRecordNewCue(sceneID_f t_id)
{
  if (m_selType != SelectionType::ChannelSelectionType)
  {
    qDebug() << " no channels selected"; // NOTE: black scene ?
    //    return;
  }
  auto L_channel = getSelectedChannels();
  m_cueEngine
      ->recordNewCueInMainSeq(m_cueEngine
                                  ->createScene(L_channel,
                                                t_id),
                              t_id);
}

void DmxEngine::onUpdateCurrentCue()
{
  if (m_selType != SelectionType::ChannelSelectionType)
  {
    qDebug() << " no channels selected"; // NOTE: black scene ?
    //    return;
  }
  auto L_channel = getSelectedChannels();
  m_cueEngine->updateScene(L_channel);
}

void DmxEngine::onUpdateCue(sceneID_f t_id)
{
  if (m_selType != SelectionType::ChannelSelectionType)
  {
    qDebug() << " no channels selected"; // NOTE: black scene ?
    //    return;
  }
  auto L_channel = getSelectedChannels();
  m_cueEngine->updateScene(L_channel,
                           t_id);
}

void DmxEngine::onRecordGroup(id t_id)
{

}

void DmxEngine::onGotoCue(sceneID_f t_id)
{
  m_cueEngine->setSelectedCueId(t_id);
}

void DmxEngine::onGotoStep(id t_id)
{
  m_cueEngine->setSelectedCueStep(t_id);
}

void DmxEngine::onDeleteCue(sceneID_f t_id)
{
  
}

void DmxEngine::onDeleteStep(id t_id)
{
  
}

void DmxEngine::onDeleteGroup(id t_id)
{
  
}

/******************************** DmxPatch *******************************/

void DmxPatch::clearPatch()
{
  m_MM_patch.clear();
}

bool DmxPatch::clearChannel(const id t_channelID)
{
  return m_MM_patch.remove(t_channelID);
}

bool DmxPatch::addOutputToChannel(const id t_channelID,
                                  const Uid_Id t_outputUid_Id)
{
  // check if it's not yet in the key
  auto L_Uid_Id = m_MM_patch.values(t_channelID);
  if (L_Uid_Id.contains(t_outputUid_Id))
  {
    qWarning() << "output already in the patch map";
    return false;
  }

  // check if it's not in another key
  L_Uid_Id = m_MM_patch.values();
  if (L_Uid_Id.contains(t_outputUid_Id))
  {
    removeOutput(t_outputUid_Id);
  }
  m_MM_patch.insert(t_channelID,
                    t_outputUid_Id);
  return true;
}

void DmxPatch::addOutputListToChannel(const id t_channelId,
                                      const QList<Uid_Id> t_L_outputUid_Id)
{
  for (const auto &item
       : std::as_const(t_L_outputUid_Id))
  {
    addOutputToChannel(t_channelId,
                       item);
  }
}

bool DmxPatch::removeOutput(const Uid_Id t_outputUid_Id)
{
  // check if it's in the map
  auto L_Uid_Id = m_MM_patch.values();
  if (L_Uid_Id.contains(t_outputUid_Id))
  {
    auto i = m_MM_patch.constBegin();
    while (i != m_MM_patch.constEnd())
    {
      if (i.value() == t_outputUid_Id)
      {
        m_MM_patch.remove(i.key(), i.value());
        break;
      }
      ++i;
    }
    return true;
  }
  qWarning() << "can't remove output from channel";
  return false;
}

void DmxPatch::removeOutputList(const QList<Uid_Id> t_L_outputUid_Id)
{
  for (const auto &item
       : std::as_const(t_L_outputUid_Id))
  {
    removeOutput(item);
  }

}

bool DmxPatch::removeOutputFromChannel(const id t_channelID,
                                       const Uid_Id t_outputUid_Id)
{
  // choper les values de la key, vérifier et remove
  auto L_Uid_Id = m_MM_patch.values(t_channelID);
  if (L_Uid_Id.contains(t_outputUid_Id))
  {
    m_MM_patch.remove(t_channelID,
                      t_outputUid_Id);
    return true;
  }
  qWarning() << "can't remove output from channel";
  return false;
}

void DmxPatch::removeOutputListFromChannel(const id t_channelID,
                                           const QList<Uid_Id> t_L_outputUid_Id)
{
  for (const auto &item
       : std::as_const(t_L_outputUid_Id))
  {
    removeOutputFromChannel(t_channelID,
                            item);
  }
}
