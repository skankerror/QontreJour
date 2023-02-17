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
#include "dmxmanager.h"
#include <QDebug>

DmxEngine *DmxEngine::instance()
{
  static DmxEngine inst;
  return &inst;
}

DmxEngine::~DmxEngine()
{}

DmxEngine::DmxEngine(QObject *parent)
  : QObject(parent),
    m_groupEngine(new ChannelGroupEngine(this))
{}

/****************************** ChannelGroupEngine ***********************/

ChannelGroupEngine::ChannelGroupEngine(QObject *parent):
  QObject(parent),
  m_globalGroup(new GlobalChannelGroup())
{}

ChannelGroupEngine::~ChannelGroupEngine()
{
  delete m_globalGroup;
}

bool ChannelGroupEngine::addNewGroup(const DmxChannelGroup *t_newGroup)
{
  auto L_channel = t_newGroup->getL_controledChannel();
  auto L_level = t_newGroup->getL_storedLevel();
  if (L_channel.size() != L_level.size())
  {
    qWarning() << "problem in ChannelGroupEngine::addNewGroupd";
    return false;
  }
  for (int i = 0;
       i < L_channel.size();
       i++)
  {
    m_globalGroup->addChannel(t_newGroup->getID(),
                              Id_Dmx(L_channel.at(i)->getID(),
                                     L_level.at(i)));
  }
  connect(t_newGroup,
          SIGNAL(levelChanged(id,dmx)),
          this,
          SLOT(onGroupLevelChanged(id,dmx)));
  return true;
}

bool ChannelGroupEngine::addNewGroup(const id t_groupID)
{
  auto channelGroup = GET_CHANNEL_GROUP(t_groupID);
  return addNewGroup(channelGroup);
}

bool ChannelGroupEngine::removeGroup(const DmxChannelGroup *t_group)
{
  return m_globalGroup->removeChannelGroup(t_group->getID());
  disconnect(t_group,
             SIGNAL(levelChanged(id,dmx)),
             this,
             SLOT(onGroupLevelChanged(id,dmx)));
}

bool ChannelGroupEngine::removeGroup(const id t_groupId)
{
  return m_globalGroup->removeChannelGroup(t_groupId);
}

bool ChannelGroupEngine::modifyGroup(const DmxChannelGroup *t_group)
{
  return (removeGroup(t_group) && addNewGroup(t_group));
}

bool ChannelGroupEngine::modifyGroup(const id t_groupId)
{
  return (removeGroup(t_groupId) && addNewGroup(t_groupId));
}

void ChannelGroupEngine::onGroupLevelChanged(id t_id,
                                             dmx t_level)
{
  m_globalGroup->groupLevelChanged(t_id,
                                   t_level);
}

/***************************** Global Channel Group *************************/

GlobalChannelGroup::GlobalChannelGroup()
{}

GlobalChannelGroup::~GlobalChannelGroup()
{}

void GlobalChannelGroup::addChannelGroup(id t_groupID,
                                         QList<Id_Dmx> t_L_id_dmx)
{
  for (const auto &item
       : std::as_const(t_L_id_dmx))
  {
    addChannel(t_groupID,
               item);
  }
}

bool GlobalChannelGroup::removeChannelGroup(id t_groupID)
{
  // avant il faut nettoyer m_M_channelLevel
  auto L_Id_Dmx = m_MM_totalGroup.values(t_groupID);
  for (const auto &item
       : std::as_const(L_Id_Dmx))
  {
    m_M_channelLevel.remove(item.getID());
  }
  return m_MM_totalGroup.remove(t_groupID);
}

bool GlobalChannelGroup::addChannel(const id t_groupID,
                                    const Id_Dmx t_id_dmx)
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
                            NULL_ID_DMX);
  }
  return true;
}

void GlobalChannelGroup::groupLevelChanged(const id t_groupID,
                                           const dmx t_level)
{
  // on chope les values de la key t_groupID
  auto L_Id_Dmx = m_MM_totalGroup.values(t_groupID);
  for (const auto &item
       : std::as_const(L_Id_Dmx))
  {
    auto actualId_Dmx = m_M_channelLevel.value(item.getID());
    auto newLevel = (dmx)(((float)(t_level)/255.0f)
                          * item.getLevel());

    if (newLevel > actualId_Dmx.getLevel()
        || t_groupID == actualId_Dmx.getID())
    {
        m_M_channelLevel.insert(item.getID(),
                                Id_Dmx(t_groupID,
                                       newLevel));
      // emit something ?
    }
  }
}
