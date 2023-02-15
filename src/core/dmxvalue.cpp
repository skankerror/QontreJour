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


#include "dmxvalue.h"
#include <QDebug>

/******************************** DMXVALUE **************************************/


DmxValue::DmxValue(ValueType t_type,
                   DmxValue *t_parent):
  QObject(t_parent),
  m_type(t_type)
{}

DmxValue::~DmxValue()
{}


/********************************* ROOTVALUE *************************************/

RootValue::RootValue(DmxValue::ValueType t_type,
                     DmxValue *t_parent)
  : DmxValue(t_type,
             t_parent)
{
  switch(t_type)
  {
  case DmxValue::RootOutput :
    setName("Root Output");
    break;
  case DmxValue::RootChannel :
    setName("Root Channel");
    break;
  case DmxValue::RootChannelGroup :
    setName("Root Channel Group");
    break;
  default :
    break;
  }
}

RootValue::~RootValue()
{
  for (const auto &item
       : std::as_const(m_L_childValue))
  {
    item->deleteLater();
  }
  m_L_childValue.clear();
  m_L_childValue.squeeze();
}

LeveledValue *RootValue::getChildValue(id t_id) const
{
  if ((t_id > -1)
      && (t_id < m_L_childValue.size()))
    return m_L_childValue.at(t_id);
  else
    return nullptr;
}

void RootValue::addChildValue(LeveledValue *t_value)
{
  // we check if the pointer isn't null and if the value is not in the list.
  if(t_value && !m_L_childValue.contains(t_value))
  {
    m_L_childValue.append(t_value);
    t_value->setParentValue(this);
  }
  else
    qWarning() << "cant add child value";
}

void RootValue::addChlidList(QList<LeveledValue *> t_L_value)
{
  for (const auto &item:
       std::as_const(t_L_value))
  {
    addChildValue(item);
  }
}

void RootValue::removeChildValue(id t_index)
{
  if (t_index < 0
      || t_index >= m_L_childValue.size())
  {
    auto value = m_L_childValue.at(t_index);
    value->setParentValue(nullptr);
    m_L_childValue.removeAt(t_index);
  }
  else
  {
    qWarning() << "can't remove value";
  }
}

void RootValue::removeChildValueList(QList<id> t_L_index)
{
  for (const auto item :
       std::as_const(t_L_index))
  {
    removeChildValue(item);
  }
}

/******************************** LEVELEDVALUE **************************************/

LeveledValue::LeveledValue(DmxValue::ValueType t_type,
                           RootValue *t_parent)
  : DmxValue(t_type,
             t_parent)
{}

LeveledValue::~LeveledValue()
{}

void LeveledValue::setLevel(dmx t_level)
{
  if (m_level == t_level)
    return;
  m_level = t_level;
  emit levelChanged(m_level);
}

/********************************* DMXOUTPUT *************************************/

DmxOutput::DmxOutput(DmxValue::ValueType t_type,
                     RootValue *t_parent)
  : LeveledValue(t_type,
                 t_parent)
{
  setName(DEFAULT_OUTPUT_NAME);
}

DmxOutput::~DmxOutput()
{}


/********************************** DMXCHANNEL ************************************/

DmxChannel::DmxChannel(DmxValue::ValueType t_type,
                       RootValue *t_parent)
  : LeveledValue(t_type,
                 t_parent)
{
  setName(DEFAULT_CHANNEL_NAME);
}

DmxChannel::~DmxChannel()
{
  clearControledOutput();
}

DmxOutput *DmxChannel::getControledOutput(const id t_index)
{
  if ((t_index > -1)
      && (t_index < m_L_controledOutput.size()))
    return m_L_controledOutput.at(t_index);
  else
    return nullptr;
}

dmx DmxChannel::getControledOutputLevel(const id t_index)
{
  if ((t_index > -1)
      && (t_index < m_L_controledOutput.size()))
  {
    return m_L_controledOutput.at(t_index)->getLevel();
  }
  else
  {
    qWarning() << "Problem in DmxChannel::getControledOutputLevel";
    return 0;
  }
}

void DmxChannel::addOutput(DmxOutput *t_dmxOutput)
{
  // we check if the pointer isn't null and if the value is not in the list.
  if(t_dmxOutput
     && !m_L_controledOutput.contains(t_dmxOutput))
  {
    m_L_controledOutput.append(t_dmxOutput);
//    t_dmxOutput->
  }
  else
    qWarning() << "cant add output";
}

void DmxChannel::addOutputList(const QList<DmxOutput *> t_L_controledOutput)
{
  for (const auto &item :
       std::as_const(t_L_controledOutput))
  {
    addOutput(item);
  }
}

void DmxChannel::removeOutput(const id t_index)
{
  if ((t_index > -1)
      && (t_index < m_L_controledOutput.size()))
  {
    m_L_controledOutput.removeAt(t_index);
  }
  else
  {
    qWarning() << "can't remove output";
  }
}

void DmxChannel::removeOutputList(const QList<id> t_L_index)
{
  for (const auto item
       : std::as_const(t_L_index))
  {
    removeOutput(item);
  }
}

void DmxChannel::clearControledOutput()
{
  m_L_controledOutput.clear();
  m_L_controledOutput.squeeze();
}

void DmxChannel::addChannelGroupControler(id t_id)
{
  if (m_M_channelGroup_Id_Level.contains(t_id))
  {
    qWarning() << "DmxChannel::addChannelGroupControler"
               << "id yet in the map";
    return;
  }
  m_M_channelGroup_Id_Level.insert(t_id,
                                   0);
}

void DmxChannel::addChannelGroupControlerList(QList<id> t_L_id)
{
  for (const auto item
       : std::as_const(t_L_id))
  {
    addChannelGroupControler(item);
  }
}

void DmxChannel::removeChannelGroupControler(id t_id)
{
  if (m_M_channelGroup_Id_Level.contains(t_id))
  {
    m_M_channelGroup_Id_Level.remove(t_id);
    return;
  }
  qWarning() << "DmxChannel::addChannelGroupControler"
             << "id yet in the map";
}

void DmxChannel::removeChannelGroupControlerList(QList<id> t_L_id)
{
  for (const auto item
       : std::as_const(t_L_id))
  {
    removeChannelGroupControler(item);
  }
}


/********************************** DMXCHANNELGROUP ************************************/

DmxChannelGroup::DmxChannelGroup(DmxValue::ValueType t_type,
                                 RootValue *t_parent)
  : LeveledValue(t_type,
                 t_parent)
{
  setName(DEFAULT_GROUP_NAME);
}

DmxChannelGroup::~DmxChannelGroup()
{
  clearControledChannel();
}

DmxChannel *DmxChannelGroup::getControledChannel(const id t_index)
{
  if ((t_index > -1)
      && (t_index < m_L_controledChannel.size()))
    return m_L_controledChannel.at(t_index);
  else
    return nullptr;

}

dmx DmxChannelGroup::getControledChannelLevel(const id t_index)
{
  if ((t_index > -1)
      && (t_index < m_L_controledChannel.size()))
  {
    return m_L_controledChannel.at(t_index)->getLevel();
  }
  else
  {
    qWarning() << "Problem in DmxChannelGroup::getControledChannelLevel";
    return 0;
  }
}

dmx DmxChannelGroup::getControledChannelStoredLevel(const id t_index)
{
  if ((t_index > -1)
      && (t_index < m_L_controledChannel.size())
      && (t_index < m_L_storedLevel.size()))
  {
    return m_L_storedLevel.at(t_index);
  }
  else
  {
    qWarning() << "Problem in DmxChannelGroup::getControledChannelStoredLevel";
    return 0;
  }
}

void DmxChannelGroup::addChannel(DmxChannel *t_dmxChannel,
                                 dmx t_storedLevel)
{
  // we check if the pointer isn't null and if the value is not in the list.
  if(t_dmxChannel
     && !m_L_controledChannel.contains(t_dmxChannel))
  {
    m_L_controledChannel.append(t_dmxChannel);
    m_L_storedLevel.append((t_storedLevel));
    t_dmxChannel->addChannelGroupControler(m_ID);
  }
  else
    qWarning() << "cant add channel";

}

void DmxChannelGroup::addChannelList(QList<DmxChannel *> t_L_controledChannel,
                                     QList<dmx> t_L_storedLevel)
{
  for (int i = 0;
       i < t_L_controledChannel.size()
       || i < t_L_storedLevel.size();
       i++)
  {
    addChannel(t_L_controledChannel.at(i),
               t_L_storedLevel.at(i));
  }
}

// NOTE : warning this id represent the number in the list and the
// channel id.
void DmxChannelGroup::removeChannel(const id t_index)
{
  if ((t_index > -1)
      && (t_index < m_L_controledChannel.size())
      && (t_index < m_L_storedLevel.size()))
  {
    auto channel = m_L_controledChannel.at(t_index);
    channel->removeChannelGroupControler(m_ID);
    m_L_controledChannel.removeAt(t_index);
    m_L_storedLevel.removeAt(t_index);
  }
  else
  {
    qWarning() << "can't remove channel";
  }
}

void DmxChannelGroup::removeChannelList(const QList<id> t_L_index)
{
  for (const auto item
       : std::as_const(t_L_index))
  {
    removeChannel(item);
  }
}

void DmxChannelGroup::clearControledChannel()
{
  m_L_controledChannel.clear();
  m_L_controledChannel.squeeze();
  m_L_storedLevel.clear();
  m_L_storedLevel.squeeze();
}


