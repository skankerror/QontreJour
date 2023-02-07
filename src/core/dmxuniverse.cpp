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

#include "dmxuniverse.h"
#include <QDebug>

DmxUniverse::DmxUniverse(uid t_universeID,
                         int t_outputCount/* = 512 */,
                         QObject *parent)
  : QObject(parent),
    m_ID(t_universeID), // first universe will have id 0
    m_outputCount(t_outputCount),
    m_isConnected(false),
    m_dmxDevice(nullptr),
    m_rootChannel(new DmxValue(DmxValue::RootChannel)),
    m_rootOutput(new DmxValue(DmxValue::RootOutput))
{
  m_rootChannel->setUniverseID(m_ID);
  m_rootOutput->setUniverseID(m_ID);

  for (int i = 0;
       i < m_outputCount;
       i++)
  {
    // create output and channel
    auto dmxOutput = new DmxValue(DmxValue::Output,
                                  m_rootOutput);
    dmxOutput->setUniverseID(m_ID);
    dmxOutput->setID(i);

    auto dmxChannel = new DmxValue(DmxValue::Channel,
                                   m_rootChannel);
    dmxChannel->setUniverseID(m_ID);
    dmxChannel->setID(i);

    // we start with straight patch
    dmxChannel->addControledChild(dmxOutput);

    m_rootChannel->addChildValue(dmxChannel);
    m_rootOutput->addChildValue(dmxOutput);

    connect(dmxChannel,
            SIGNAL(levelChanged(DmxValue::SignalSenderType,dmx)),
            dmxOutput,
            SLOT(setLevel(DmxValue::SignalSenderType,dmx)));

    connect(dmxOutput,
            SIGNAL(requestDmxUpdate(id,dmx)),
            this,
            SLOT(onRequestDmxUpdate(id,dmx)));


  }
}

DmxUniverse::~DmxUniverse()
{
  // détruire output et channel !
  m_rootChannel->deleteLater();
  m_rootOutput->deleteLater();
}

void DmxUniverse::onRequestDmxUpdate(id t_ID,
                                     dmx t_level)
{
  if (!m_isConnected)
    return;

  QDmxManager::instance()->writeData(m_ID,
                                     t_ID,
                                     t_level);

  qDebug() << "writeData(" << m_ID << t_ID << t_level << ")";

}

