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

DmxManager::DmxManager(QObject *parent)
  : QObject(parent),
    m_hwManager(QDmxManager::instance()),
    m_dmxPatch(new DmxPatch()),
    m_rootChannel(new RootValue(ValueType::RootChannel)),
    m_rootChannelGroup(new RootValue(ValueType::RootChannelGroup))
{
  // init hardware manager
  m_hwManager->init();

  // we create first universe
  auto universe = new DmxUniverse(0);
  m_L_universe.append(universe);

  // connection to hardware output
  connectOutputs();

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

  m_dmxEngine = new DmxEngine(m_rootChannelGroup,
                              m_rootChannel,
                              getL_rootOutput(),
                              m_dmxPatch,
                              this);

  m_interpreter = new Interpreter(this);
  connectInterpreter();

}

void DmxManager::testingMethod()
{
  //test
  auto channel1 = getChannel(10);
  auto channel2 = getChannel(11);
  channel1->setLevel(100);
  channel2->setLevel(147);

  auto L_channel = QList<DmxChannel *>();
  L_channel.append(channel1);
  L_channel.append(channel2);
  createChannelGroup(L_channel);

  getChannelGroup(0)->setLevel(100);
  getChannelGroup(0)->setLevel(0);

  connectValueToWidget(WidgetType::DmxSlider,
                       0,
                       ValueType::ChannelGroup,
                       0);

}

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
  delete m_dmxPatch;
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

DmxOutput *DmxManager::getOutput(Uid_Id t_output_Uid_Id)
{
  auto universeID = t_output_Uid_Id.getUniverseID();
  if (universeID < 0
      || universeID >= getUniverseCount())
  {
    qWarning() << "problem in DmxManager::getOutput";
    return nullptr;
  }
  auto rootOutput = getRootOutput(universeID);
  auto outputID = t_output_Uid_Id.getOutputID();
  if (outputID < 0
      || outputID >= rootOutput->getL_childValueSize())
  {
    qWarning() << "problem in DmxManager::getOutput";
    return nullptr;
  }
  return static_cast<DmxOutput *>(rootOutput->getChildValue(outputID));
}

DmxChannel *DmxManager::getChannel(id t_channelId)
{
  if (t_channelId < 0
      || t_channelId >= getChannelCount())
  {
    qWarning() << "problem in DmxManager::getChannel";
    return nullptr;
  }
  return static_cast<DmxChannel *>(m_rootChannel->getChildValue(t_channelId));
}

DmxChannelGroup *DmxManager::getChannelGroup(id t_groupId)
{
  if (t_groupId < 0
      || t_groupId >= getChannelGroupCount())
  {
    qWarning() << "problem in DmxManager::getChannelGroup";
    return nullptr;
  }
  return static_cast<DmxChannelGroup *>(m_rootChannelGroup->getChildValue(t_groupId));
}

bool DmxManager::createUniverse(uid t_universeID)
{
  if (t_universeID == getUniverseCount())
  {
    auto universe = new DmxUniverse(t_universeID);
    m_L_universe.append(universe);

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

    return true;
  }
  return false;
}

DmxChannelGroup *DmxManager::createChannelGroup(QList<DmxChannel *> t_L_channel)
{
  auto newGroup = new DmxChannelGroup(ValueType::ChannelGroup);
  newGroup->setID(getChannelGroupCount());
  auto H_controledChannel_storedLevel = QHash<DmxChannel *,dmx>();
  for (const auto item
       : std::as_const(t_L_channel))
  {
    H_controledChannel_storedLevel.insert(item,
                                          item->getLevel());
  }
  newGroup->setH_controledChannel_storedLevel(H_controledChannel_storedLevel);
  m_rootChannelGroup->addChildValue(newGroup);

  m_dmxEngine->getGroupEngine()->addNewGroup(newGroup);

  return newGroup;
}

RootValue *DmxManager::getRootOutput(const uid t_uid) const
{
  if (t_uid < 0
      || t_uid >= m_L_universe.size())
  {
    qWarning() << " can't DmxOutput *DmxManager::getRootOutput(const uid t_uid)";
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

void DmxManager::connectOutputs()
{
  auto L_rootOutput = getL_rootOutput();
  for (const auto &i
       : std::as_const(L_rootOutput))
  {
    auto L_output = i->getL_childValue();
    for (const auto &j
         : std::as_const(L_output))
    {
      auto output = static_cast<DmxOutput *>(j);
      connect(output,
              SIGNAL(outputRequestUpdate(uid,id,dmx)),
              this,
              SLOT(onOutputRequest(uid,id,dmx)));
    }
  }
}

void DmxManager::connectInterpreter()
{
  connect(m_interpreter,
          &Interpreter::addChannelSelection,
          this,
          &DmxManager::onAddChannelSelection);

  connect(m_interpreter,
          &Interpreter::removeChannelSelection,
          this,
          &DmxManager::onRemoveChannelSelection);

  connect(m_interpreter,
          &Interpreter::addOutputSelection,
          this,
          &DmxManager::onAddOutputSelection);

  connect(m_interpreter,
          &Interpreter::removeOutputSelection,
          this,
          &DmxManager::onRemoveOutputSelection);

  connect(m_interpreter,
          &Interpreter::selectAll,
          this,
          &DmxManager::onSelectAll);

  connect(m_interpreter,
          &Interpreter::clearChannelSelection,
          this,
          &DmxManager::onClearChannelSelection);

  connect(m_interpreter,
          &Interpreter::clearOutputSelection,
          this,
          &DmxManager::onClearOutputSelection);

  connect(m_interpreter,
          &Interpreter::setLevel,
          this,
          &DmxManager::onSetLevel);

  connect(m_interpreter,
          &Interpreter::sendError,
          this,
          &DmxManager::onSendError);

  connect(m_interpreter,
          &Interpreter::sendError_NoValueSpecified,
          this,
          &DmxManager::onSendError_NoValueSpecified);
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
  auto biggerSize = m_rootChannel->getL_childValueSize()
      < rootOutput->getL_childValueSize()
      ? m_rootChannel->getL_childValueSize()
      : rootOutput->getL_childValueSize();
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
      qWarning() << " can't DmxManager::setStraightPatch(const QList<uid>)";
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
    totalOutputSize += item->getL_childValueSize();
    L_output += item->getL_childValue();
  }
  auto biggerSize = m_rootChannel->getL_childValueSize()
      < totalOutputSize
      ? m_rootChannel->getL_childValueSize()
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
  // TODO : patch all universes
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
    qWarning() << "can't DmxManager::unpatchOutputFromChannel";
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
    qWarning() << "can't DmxManager::unpatchOutput";
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
    qWarning() << "can't DmxManager::clearChannelPatch";
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

void DmxManager::connectValueToWidget(WidgetType t_widgetType,
                                      int t_widgetID,
                                      ValueType t_valueType,
                                      id t_valueID)
{
  if (t_widgetType == DmxSlider)
  {
    if (t_valueType == ValueType::ChannelGroup)
    {
      emit connectGroupToSubmasterSlider(t_widgetID,
                                         t_valueID);
    }
    else if (t_valueType == ValueType::ChannelType)
    {
      emit connectChannelToDirectChannelSlider(t_widgetID,
                                         t_valueID);
    }
  }
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

void DmxManager::submasterToEngine(id t_id,
                                   dmx t_level)
{
  m_dmxEngine->getGroupEngine()->groupLevelChanged(t_id,
                                                   t_level);
}

void DmxManager::directChannelToEngine(id t_id,
                                       dmx t_level)
{
  m_dmxEngine->getChannelEngine()->onChannelLevelChangedFromDirectChannel(t_id,
                                                                          t_level);
}

void DmxManager::keypadToInterpreter(KeypadButton t_buttonType)
{
  m_interpreter->recieveData(t_buttonType);
}


void DmxManager::onOutputRequest(uid t_uid,
                                 id t_id,
                                 dmx t_level)
{
  m_hwManager->writeData(t_uid,
                         t_id,
                         t_level);

  qDebug() << "write uid :"  << t_uid
           << "id :" << t_id
           << "level :" << t_level;

}

void DmxManager::onAddChannelSelection(QList<id> t_L_id)
{
  for (qsizetype i = 0;
       i < t_L_id.size();
       i++)
  {
    id channelId = t_L_id.at(i);
    if (m_L_channelsIdSelection.indexOf(channelId) == -1)
    {
      m_L_channelsIdSelection.append(channelId);
      emit ChannelSelectionChanged();
    }
  }
}

void DmxManager::onRemoveChannelSelection(QList<id> t_L_id)
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
      emit ChannelSelectionChanged();
    }
  }
}

void DmxManager::onAddOutputSelection(QList<Uid_Id> t_L_Uid_Id)
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
}

void DmxManager::onRemoveOutputSelection(QList<Uid_Id> t_L_Uid_Id)
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

void DmxManager::onSelectAll()
{

}

void DmxManager::onClearChannelSelection()
{
  m_L_channelsIdSelection.clear();
  m_L_channelsIdSelection.squeeze();
  emit ChannelSelectionChanged();
}

void DmxManager::onClearOutputSelection()
{
  m_L_outputUid_IdSelection.clear();
  m_L_outputUid_IdSelection.squeeze();
}

void DmxManager::onSetLevel(dmx t_level)
{

}

void DmxManager::onSendError()
{

}

void DmxManager::onSendError_NoValueSpecified()
{

}


