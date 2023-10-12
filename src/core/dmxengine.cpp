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

/******************************** ChannelData *****************************/

void ChannelData::update()
{
  if (m_flag == ChannelDataFlag::DirectChannelFlag)
  {
    if (m_channelGroupLevel > m_directChannelLevel)
    {
      if (m_channelGroupLevel > m_actual_Level)
        m_actual_Level = m_channelGroupLevel;
      //      emit blockChannelSlider(m_actual_Level);
      m_flag = ChannelDataFlag::ChannelGroupFlag;
    }
    else
    {
      //      if (m_directChannelLevel > m_actual_Level)
      m_actual_Level = m_directChannelLevel;
      //      emit blockChannelSlider(m_actual_Level);
      m_flag = ChannelDataFlag::DirectChannelFlag;
    }
  }
  else
  {
    if (m_channelGroupLevel >= m_sceneLevel)
    {
      m_actual_Level = m_channelGroupLevel;
      m_flag = ChannelDataFlag::ChannelGroupFlag;
    }
    else
    {
      if (m_sceneLevel > m_actual_Level)
        m_actual_Level = m_sceneLevel;
      m_flag = ChannelDataFlag::SelectedSceneFlag;
    }
  }
}

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
    addChannel(t_newGroup->getID(),
               Ch_Id_Dmx(i.key()->getID(),
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
  return removeChannelGroup(t_group->getID());
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
    m_M_channelLevel.remove(item.getID());
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
  if (!(m_M_channelLevel.contains(t_id_dmx.getID())))
  {
    m_M_channelLevel.insert(t_id_dmx.getID(),
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
    auto actualCh_Id_Dmx = m_M_channelLevel.value(item.getID());
    auto newLevel = (dmx)(((float)(t_level)/255.0f)
                          * item.getLevel());

    if (newLevel > actualCh_Id_Dmx.getLevel()
        || t_groupID == actualCh_Id_Dmx.getID())
    {
      m_M_channelLevel.insert(item.getID(),
                              Gr_Id_Dmx(t_groupID,
                                        newLevel));
      // emit to channel engine
      emit channelLevelChangedFromGroup(item.getID(),
                                        newLevel);
    }
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
//    delete item;
    item->deleteLater();
  }
  m_L_channelData.clear();
}

void ChannelEngine::createDatas(int t_channelCount)
{
  for (int i = 0;
       i < t_channelCount;
       i++)
  {
    auto channelData = new ChannelData(i);
    m_L_channelData.append(channelData);
    auto channel = /*GET_CHANNEL(i)*/ m_rootChannel->getChildValue(i);
    connect(channelData,
            SIGNAL(blockChannelSlider(dmx)),
            channel,
            SLOT(setLevel(dmx)));
  }
}

void ChannelEngine::update(id t_id)
{
  auto channelData = m_L_channelData.at(t_id);
  auto channel = GET_CHANNEL(t_id);
  channelData->update();
  channel->setLevel(channelData->getActual_Level());
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
  channelData->setFlag(ChannelDataFlag::DirectChannelFlag);
  channelData->setDirectChannelLevel(t_level);
  channelData->setDirectChannelOffset(t_offset);
  update(t_id);
}

void ChannelEngine::onChannelLevelChangedFromScene(id t_id,
                                                   dmx t_level)
{

}

void ChannelEngine::onChannelLevelChangedFromNextScene(id t_id,
                                                       dmx t_level)
{

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
  qDebug() << "channel id" << t_channelId << "level" << t_level;
  auto L_Uid_Id = m_patch->getL_Uid_Id(t_channelId);
  for (const auto &i
       : std::as_const(L_Uid_Id))
  {
    auto rootOutput = m_L_rootOutput.at(i.getUniverseID());
    auto output = rootOutput->getChildValue(i.getOutputID());
    output->setLevel(t_level);
  }
}

/******************************* DmxEngine ***************************/

DmxEngine::DmxEngine(RootValue *t_rootGroup,
                     RootValue *t_rootChannel,
                     QList<RootValue *> t_L_rootOutput,
                     DmxPatch *t_patch,
                     QObject *parent)
    : QObject(parent),
    m_groupEngine(new ChannelGroupEngine(t_rootGroup,
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
  m_channelEngine->deleteLater();
  m_outputEngine->deleteLater();
}

