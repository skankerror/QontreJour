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

#include "dmxmanager.h"
#include <QDebug>

DmxManager *DmxManager::instance()
{
  static DmxManager inst;
  return &inst;
}

DmxManager::~DmxManager()
{
  m_hwManager->teardown();
  m_rootChannel->deleteLater();
  m_rootChannelGroup->deleteLater();
  for (const auto &item
       : std::as_const(m_L_universe))
  {
    item->deleteLater();
  }
  m_L_universe.clear();
  m_L_universe.squeeze();
}

QStringList DmxManager::getAvailableDriversNames() const
{
  auto retList = QStringList();
  auto driversList = getAvailableDrivers();
  for (auto item : driversList)
  {
    retList.append(item->name());
  }
  return retList;
}

QStringList DmxManager::getAvailableDevicesNames(const QString &t_driverString)
{
  auto retList = QStringList();
  auto devicesList = getAvailableDevices(t_driverString);
  for (auto item : devicesList)
  {
    retList.append(item->name());
  }
  return retList;
}

bool DmxManager::createUniverse(uid t_universeID)
{
  if (t_universeID == getUniverseCount())
  {
    auto universe = new DmxUniverse(t_universeID);
    m_L_universe.append(universe);

    connect(universe,
            SIGNAL(universeRequireUpdate(uid,id,dmx)),
            this,
            SLOT(onUniverseRequestUpdate(uid,id,dmx)));

    return true;
  }
  else if (t_universeID < getUniverseCount())
  {
    return false;
  }
  else if (t_universeID > getUniverseCount())
  {
    auto universe = new DmxUniverse(getUniverseCount());
    qDebug() << "universe id asked is too much high";
    m_L_universe.append(universe);

    connect(universe,
            SIGNAL(universeRequireUpdate(uid,id,dmx)),
            this,
            SLOT(onUniverseRequestUpdate(uid,id,dmx)));

    return true;
  }
  return false;
}

DmxChannelGroup *DmxManager::createChannelGroup(QList<DmxChannel *> t_L_channel)
{
  auto newGroup = new DmxChannelGroup(DmxValue::ChannelGroup);
  newGroup->setID(getChannelGroupCount());
  newGroup->setL_controledChannel(t_L_channel);
  auto L_storedLevel = QList<dmx>();
  for (const auto item
       : std::as_const(t_L_channel))
  {
    auto level = item->getLevel();
    L_storedLevel.append(level);
  }
  newGroup->setL_storedLevel(L_storedLevel);
  m_rootChannelGroup->addChildValue(newGroup);

  return newGroup;
}

RootValue *DmxManager::getRootOutput(const uid t_uid) const
{
  if (t_uid < 0
      || t_uid >= m_L_universe.size())
  {
    qWarning () << " can't DmxOutput *DmxManager::getRootOutput(const uid t_uid)";
    return nullptr;
  }
  return m_L_universe.at(t_uid)->getRootOutput();
}

QList<RootValue *> DmxManager::getL_rootOutput() const
{
  QList<RootValue *> L_rootOutput;
  for (const auto &item
       : std::as_const(m_L_universe))
  {
    L_rootOutput.append(item->getRootOutput());
  }
  return L_rootOutput;
}

void DmxManager::setStraightPatch(const uid t_uid)
{
  clearPatch();
  if (t_uid < 0
      || t_uid >= m_L_universe.size())
  {
    qWarning () << " can't DmxManager::setStraightPatch(const uid)";
    return;
  }
  auto rootOutput = getRootOutput(t_uid);
  auto biggerSize = m_rootChannel->getL_ChildValueSize()
      < rootOutput->getL_ChildValueSize()
      ? m_rootChannel->getL_ChildValueSize()
      : rootOutput->getL_ChildValueSize();
  for (int i = 0;
       i < biggerSize;
       i++)
  {
    auto channel = m_rootChannel->getChildValue(i);
    auto channelCast = static_cast<DmxChannel *>(channel);
    auto output = rootOutput->getChildValue(i);
    auto outputCast = static_cast<DmxOutput *>(output);
    patchOutputToChannel(channelCast,
                         outputCast);
  }

}


void DmxManager::setStraightPatch(const QList<uid> t_L_uid)
{
  clearPatch();

  // get output list
  QList<RootValue *> L_rootOutput;
  for (const auto item
       : std::as_const(t_L_uid))
  {
    if (item < 0
        || item >= m_L_universe.size())
    {
      qWarning () << " can't DmxManager::setStraightPatch(const QList<uid>)";
      return;
    }
    L_rootOutput.append(getRootOutput(item));
  }
  int totalOutputSize = 0;
  // for agregate all outputs and patch in the order
  QList<LeveledValue *> L_output;
  for (const auto &item
       : std::as_const(L_rootOutput))
  {
    totalOutputSize += item->getL_ChildValueSize();
    L_output += item->getL_childValue();
  }
  auto biggerSize = m_rootChannel->getL_ChildValueSize()
      < totalOutputSize
      ? m_rootChannel->getL_ChildValueSize()
      : totalOutputSize;

  for (int i = 0;
       i < biggerSize;
       i++)
  {
    auto channel = m_rootChannel->getChildValue(i);
    auto channelCast = static_cast<DmxChannel *>(channel);
    auto output = L_output.at(i);
    auto outputCast = static_cast<DmxOutput *>(output);
    patchOutputToChannel(channelCast,
                         outputCast);
  }
}

void DmxManager::setStraightPatch()
{

}

void DmxManager::clearPatch()
{
  m_dmxPatch->clearPatch();

  auto L_Channel = m_rootChannel->getL_childValue();
  for (const auto &item
       : std::as_const(L_Channel))
  {
    auto channel = static_cast<DmxChannel *>(item);
    auto L_output = channel->getL_controledOutput();
    for (const auto &it
         : std::as_const(L_output))
    {
      it->setChannelControler(nullptr);
    }
    channel->clearControledOutput();
  }

}

void DmxManager::patchOutputToChannel(DmxChannel *t_channel,
                                      DmxOutput *t_output)
{
  auto channelID = t_channel->getID();
  auto outputUid_Id = Uid_Id(t_output);
  if (m_dmxPatch->addOutputToChannel(channelID,
                                     outputUid_Id))
  {
    t_channel->addOutput(t_output);
    t_output->setChannelControler(t_channel);
  }
  else
  {
    qWarning() << "can't DmxManager::patchOutputToChannel";
  }

}

void DmxManager::patchOutputListToChannel(DmxChannel *t_channel,
                                          QList<DmxOutput *> t_L_output)
{
  for (const auto &item
       : std::as_const(t_L_output))
  {
    patchOutputToChannel(t_channel,
                         item);
  }
}

void DmxManager::unpatchOutputFromChannel(DmxChannel *t_channel,
                                          DmxOutput *t_output)
{
  auto channelID = t_channel->getID();
  auto outputUid_Id = Uid_Id(t_output);
  if (m_dmxPatch->removeOutputFromChannel(channelID,
                                          outputUid_Id))
  {
    t_channel->removeOutput(t_output);
    t_output->setChannelControler(nullptr);
  }
  else
  {
    qWarning () << "can't DmxManager::unpatchOutputFromChannel";
  }
}

void DmxManager::unpatchOutputListFromChannel(DmxChannel *t_channel,
                                              QList<DmxOutput *> t_L_output)
{
  for (const auto &item
       : std::as_const(t_L_output))
  {
    unpatchOutputFromChannel(t_channel,
                             item);
  }
}

void DmxManager::unpatchOutput(DmxOutput *t_output)
{
  auto outputUid_Id = Uid_Id(t_output);
  if (m_dmxPatch->removeOutput(outputUid_Id))
  {
    auto channel = t_output->getChannelControler();
    channel->removeOutput(t_output);
    t_output->setChannelControler(nullptr);
  }
  else
  {
    qWarning () << "can't DmxManager::unpatchOutput";
  }
}

void DmxManager::unpatchOutputList(QList<DmxOutput *> t_L_output)
{
  for (const auto &item
       : std::as_const(t_L_output))
  {
    unpatchOutput(item);
  }
}

void DmxManager::clearChannelPatch(DmxChannel *t_channel)
{
  if (m_dmxPatch->clearChannel(t_channel->getID()))
  {
    auto L_output = t_channel->getL_controledOutput();
    for (const auto &item
         : std::as_const(L_output))
    {
      item->setChannelControler(nullptr);
    }
    t_channel->clearControledOutput();
  }
  else
  {
    qWarning () << "can't DmxManager::clearChannelPatch";
  }

}

void DmxManager::clearChannelListPatch(QList<DmxChannel *> t_L_channel)
{
  for (const auto &item
       : std::as_const(t_L_channel))
  {
    clearChannelPatch(item);
  }
}

bool DmxManager::hwConnect(HwPortType t_type,
                           QString &t_driver,
                           QString &t_device,
                           quint8 t_port,
                           uid t_ID)
{
  //  if (t_type == DmxManager::HwOutput)
  if(m_hwManager->patch(QDmxManager::Output,
                        t_driver,
                        t_device,
                        t_port,
                        t_ID))
  {
    m_L_universe.at(t_ID)->setConnected(true);
    return true;
  }
  else
    return false;
}

bool DmxManager::hwDisconnect(uid t_ID)
{
  if (m_hwManager->unpatch(t_ID))
  {
    m_L_universe.at(t_ID)->setConnected(false);
    return true;
  }
  else
    return false;
}

QList<QDmxDriver *> DmxManager::getAvailableDrivers() const
{
  return m_hwManager->availableDrivers();
}

QList<QDmxDevice *> DmxManager::getAvailableDevices(const QString &t_driverString)
{
  auto driver = m_hwManager->driver(t_driverString);
  driver->setEnabled(true);
  return driver->availableDevices();
}

//void DmxManager::onUniverseRequestUpdate(uid t_uid,
//                                         id t_id,
//                                         dmx t_level)
//{
//  m_hwManager->writeData(t_uid,
//                         t_id,
//                         t_level);

//  qDebug() << "write uid :"  << t_uid
//           << "id :" << t_id
//           << "level :" << t_level;

//}

DmxManager::DmxManager(QObject *parent)
  : QObject(parent),
    m_hwManager(QDmxManager::instance()),
    m_dmxPatch(new DmxPatch()),
    m_rootChannel(new RootValue(DmxValue::RootChannel)),
    m_rootChannelGroup(new RootValue(DmxValue::RootChannelGroup))
{
  // init hardware manager
  m_hwManager->init();

  // we create first universe
  auto universe = new DmxUniverse(0);
  m_L_universe.append(universe);

  // create default number of channels
  for (int i = 0;
       i < DEFAULT_CHANNEL_COUNT;
       i++)
  {
    auto channel = new DmxChannel();
    channel->setID(i);
    m_rootChannel->addChildValue(channel);
  }

  // start with straight patch between our channels
  // and outputs from 1st universe

  setStraightPatch(0); // patch first universe

}

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
    qWarning () << "output already in the patch map";
    return false;
  }

  // chack if it's not in another key
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
  qWarning () << "can't remove output from channel";
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
  qWarning () << "can't remove output from channel";
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

