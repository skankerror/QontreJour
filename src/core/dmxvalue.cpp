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
#include "dmxmanager.h"
#include <QDebug>

/*************************** Uid_Id /************************************/

Uid_Id::Uid_Id(const DmxOutput *t_output)
{
  if (t_output)
  {
    m_universeID = t_output->getUniverseID();
    m_outputID = t_output->getID();
  }
}

Uid_Id::Uid_Id(const QString &t_string)
{
  if (t_string.size() > 3
      && t_string.at(0).isDigit())
  {
    auto tempFloat = t_string.toFloat();
    m_universeID = (uid)tempFloat;
    // TODO : implement this fucking string method
//    for (qsizetype i = 0;
//         i < t_string.size();
//         i++)
//    {
//      auto value = t_string.at(i);
//      if (value.isDigit())

  }
}

/******************************** DMXVALUE **************************************/


DmxValue::DmxValue(ValueType t_type,
                   DmxValue *t_parent)
  : QObject(t_parent),
  m_type(t_type)
{}

DmxValue::DmxValue(const DmxValue &t_value)
  : QObject(t_value.parent()),
    m_type(t_value.m_type),
    m_ID(t_value.m_ID),
    m_universeID(t_value.m_universeID),
    m_name(t_value.m_name)
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
  if ((t_id > NO_ID)
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

void RootValue::addChlidList(const QList<LeveledValue *> t_L_value)
{
  for (const auto &item:
       std::as_const(t_L_value))
  {
    addChildValue(item);
  }
}

void RootValue::removeChildValue(const id t_index)
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

void RootValue::removeChildValueList(const QList<id> t_L_index)
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

LeveledValue::LeveledValue(const LeveledValue &t_value)
  : DmxValue(t_value),
    m_level(t_value.m_level),
    m_parentValue(t_value.m_parentValue),
    m_assignedWidget(t_value.m_assignedWidget)
{}

LeveledValue::~LeveledValue()
{}

void LeveledValue::setLevel(dmx t_level)
{
  if (m_level == t_level)
    return;
  m_level = t_level;
  emit levelChanged(m_ID,
                    m_level);
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

DmxChannel::DmxChannel(const DmxChannel &t_channel)
  : LeveledValue(t_channel),
    m_L_controledOutput(t_channel.m_L_controledOutput),
    m_directChannelEditLevel(t_channel.m_directChannelEditLevel),
    m_channelGroupLevel(t_channel.m_channelGroupLevel),
    m_selectedSceneLevel(t_channel.m_selectedSceneLevel),
    m_nextSceneLevel(t_channel.m_nextSceneLevel),
    m_overOffset(t_channel.m_overOffset),
    m_flag(t_channel.m_flag),
    m_isDirectChannelEdited(t_channel.m_isDirectChannelEdited)
{}

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
    return NULL_DMX;
  }
}

void DmxChannel::addOutput(DmxOutput *t_dmxOutput)
{
  // we check if the pointer isn't null and if the value is not in the list.
  if(t_dmxOutput
     && !m_L_controledOutput.contains(t_dmxOutput))
  {
    m_L_controledOutput.append(t_dmxOutput);
  }
  else
  {
    qWarning() << "cant add output";
  }
}

void DmxChannel::addOutput(Uid_Id t_Uid_Id)
{
  addOutput(GET_OUTPUT(t_Uid_Id.getUniverseID(),
                       t_Uid_Id.getOutputID()));
}

void DmxChannel::addOutputList(const QList<DmxOutput *> t_L_controledOutput)
{
  for (const auto &item :
       std::as_const(t_L_controledOutput))
  {
    addOutput(item);
  }
}

void DmxChannel::removeOutput(const DmxOutput *t_output)
{
  auto index = m_L_controledOutput.indexOf(t_output);
  if (index != -1)
  {
    m_L_controledOutput.removeAt(index);
  }
  else
  {
    qWarning() << "can't DmxChannel::removeOutput";
  }
}

void DmxChannel::removeOutput(const Uid_Id t_Uid_Id)
{
  removeOutput(GET_OUTPUT(t_Uid_Id.getUniverseID(),
                          t_Uid_Id.getOutputID()));
}

void DmxChannel::removeOutputList(const QList<DmxOutput *> t_L_output)
{
  for (const auto &item
       : std::as_const(t_L_output))
  {
    removeOutput(item);
  }
}

void DmxChannel::removeOutputList(const QList<Uid_Id> t_L_Uid_Id)
{
  for (const auto &item
       : std::as_const(t_L_Uid_Id))
  {
    removeOutput(item);
  }
}

void DmxChannel::clearControledOutput()
{
  m_L_controledOutput.clear();
  m_L_controledOutput.squeeze();
}

//void DmxChannel::addChannelGroupControler(const id t_id)
//{
//  if (m_M_channelGroup_Id_Level.contains(t_id))
//  {
//    qWarning() << "DmxChannel::addChannelGroupControler"
//               << "id yet in the map";
//    return;
//  }
//  m_M_channelGroup_Id_Level.insert(t_id,
//                                   NULL_DMX);
//}

//void DmxChannel::addChannelGroupControlerList(const QList<id> t_L_id)
//{
//  for (const auto item
//       : std::as_const(t_L_id))
//  {
//    addChannelGroupControler(item);
//  }
//}

//void DmxChannel::removeChannelGroupControler(const id t_id)
//{
//  if (m_M_channelGroup_Id_Level.contains(t_id))
//  {
//    m_M_channelGroup_Id_Level.remove(t_id);
//    return;
//  }
//  qWarning() << "DmxChannel::addChannelGroupControler"
//             << "id yet in the map";
//}

//void DmxChannel::removeChannelGroupControlerList(const QList<id> t_L_id)
//{
//  for (const auto item
//       : std::as_const(t_L_id))
//  {
//    removeChannelGroupControler(item);
//  }
//}


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

dmx DmxChannelGroup::getControledChannelStoredLevel(const id t_id)
{
  return getControledChannelStoredLevel(GET_CHANNEL(t_id));
}

dmx DmxChannelGroup::getControledChannelStoredLevel(/*const */DmxChannel *m_channel)
{
  if (m_channel == nullptr
      || !m_H_controledChannel_storedLevel.contains(m_channel))
  {
    qWarning() << " problem in DmxChannelGroup::getControledChannelStoredLeve";
    return NULL_DMX;
  }
  return m_H_controledChannel_storedLevel.value(m_channel);
}

// NOTE : this method modifies level if channel was already in group
void DmxChannelGroup::addChannel(DmxChannel *t_dmxChannel,
                                 const dmx t_storedLevel)
{
  // we check if the pointer isn't null.
  if(t_dmxChannel)
  {
    m_H_controledChannel_storedLevel.insert(t_dmxChannel,
                                            t_storedLevel);
//    t_dmxChannel->addChannelGroupControler(m_ID);
  }
  else
    qWarning() << "cant DmxChannelGroup::addChannel";

}

void DmxChannelGroup::addChannel(const id t_id,
                                 const dmx t_storedLevel)
{
  auto channel = GET_CHANNEL(t_id);
  addChannel(channel,
             t_storedLevel);
}

//void DmxChannelGroup::addChannelList(QList<DmxChannel *> t_L_controledChannel,
//                                     QList<dmx> t_L_storedLevel)
//{
//  for (int i = 0;
//       i < t_L_controledChannel.size()
//       || i < t_L_storedLevel.size();
//       i++)
//  {
//    addChannel(t_L_controledChannel.at(i),
//               t_L_storedLevel.at(i));
//  }
//}

void DmxChannelGroup::removeChannel(DmxChannel *t_channel)
{
  if (!m_H_controledChannel_storedLevel.contains(t_channel))
  {
    qWarning() << "can't DmxChannelGroup::removeChannel";
    return;
  }
  m_H_controledChannel_storedLevel.remove(t_channel);
//  t_channel->removeChannelGroupControler(m_ID);
}

void DmxChannelGroup::removeChannel(const id t_id)
{
  removeChannel(GET_CHANNEL(t_id));
}

void DmxChannelGroup::removeChannelList(const QList<DmxChannel *> t_L_channel)
{
  for (const auto &item
       : std::as_const(t_L_channel))
  {
    removeChannel(item);
  }
}

void DmxChannelGroup::removeChannelList(const QList<id> t_L_id)
{
  for (const auto item
       : std::as_const(t_L_id))
  {
    removeChannel(item);
  }
}

void DmxChannelGroup::clearControledChannel()
{
  m_H_controledChannel_storedLevel.clear();
}


