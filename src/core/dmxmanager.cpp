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
//#include "../gui/valueeditwidget.h"

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

//bool DmxManager::createSequence()
//{
//  auto rootScene = new DmxScene(DmxValue::RootScene);
//  m_L_rootScene.append(rootScene);
//  return true;
//}

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

//void DmxManager::connectValueToWidget(WidgetType t_widgetType,
//                                      int t_widgetID,
//                                      DmxValue::ValueType t_valueType,
//                                      id t_valueID)
//{
////  if (t_widgetType == DmxSlider)
//  {
//    if (t_valueType == DmxValue::ChannelGroup)
//    {
//      emit connectGroupToSubmasterSlider(t_widgetID,
//                                         t_valueID);
//    }
//  }
//}

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

//void DmxManager::updateSubmasters()
//{
//  auto L_channelGroup = m_rootChannelGroup->getL_childValue();
//  for (const auto &item : std::as_const(L_channelGroup))
//  {
//    item->setLevel(DmxValue::SubmasterSliderSender,
//                   item->getLevel());
//  }
//}

void DmxManager::onUniverseRequestUpdate(uid t_uid,
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

DmxManager::DmxManager(QObject *parent)
  : QObject(parent),
    m_hwManager(QDmxManager::instance()),
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

  // and the main sequence :
//  auto rootScene = new DmxScene(DmxValue::RootScene);
//  m_L_rootScene.append(rootScene);


  connect(universe,
          SIGNAL(universeRequireUpdate(uid,id,dmx)),
          this,
          SLOT(onUniverseRequestUpdate(uid,id,dmx)));

  //test

}
