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

DmxUniverse::DmxUniverse(int t_universeID,
                         int t_outputCount/* = 512 */,
                         QObject *parent)
  : QObject(parent),
    m_ID(t_universeID), // first universe will have id 0
    m_outputCount(t_outputCount),
    m_isConnected(false),
    m_dmxDevice(nullptr)
{
  for (int i = 0; i < m_outputCount; i++)
  {
    // create output and channel
    auto dmxOutput = new DmxValue(m_ID,
                                  i, // first output will be id 0
                                  this);
    dmxOutput->setType(DmxValue::Output);
    auto dmxChannel = new DmxValue(m_ID,
                                   i,
                                   this);
    dmxChannel->setType(DmxValue::Channel);
    // we start with straight patch
    dmxChannel->addDmxValue(dmxOutput);
    m_L_dmxChannel.append(dmxChannel);
    m_L_dmxOutput.append(dmxOutput);

    connect(dmxChannel,
            SIGNAL(levelChanged(int,quint8)),
            dmxOutput,
            SLOT(setLevel(int,quint8)));

    connect(dmxOutput,
            SIGNAL(levelChanged(int,quint8)),
            this,
            SLOT(onOutputLevelChanged(int,quint8)));


  }
}

DmxUniverse::~DmxUniverse()
{
  // TODO : check that !

  // détruire output et channel !
  for (const auto &item : std::as_const(m_L_dmxOutput))
  {
    item->deleteLater();
  }

  for (const auto &item : std::as_const(m_L_dmxChannel))
  {
    item->deleteLater();
  }

  m_L_dmxOutput.clear();
  m_L_dmxOutput.squeeze();
  m_L_dmxChannel.clear();
  m_L_dmxChannel.squeeze();
//  if (DmxUniverse::isConnected)
//    m_dmxDevice->deleteLater();

}

//bool DmxUniverse::setOutputLevel(int t_outputID,
//                                 int t_level)
//{// TODO : en a-t-on vraiment besoin ?
//  if (t_outputID < m_outputCount)
//  {
//    auto dmxOutput = m_L_dmxOutput.at(t_outputID);
//    dmxOutput->setLevel(t_level);
//    return true;
//  }
//  qWarning() << "try to set an output out of range in this universe";
//  return false;
//}

void DmxUniverse::onOutputLevelChanged(int t_outputID, quint8 t_level)
{
  emit dmxOutputUpdateRequired(t_outputID,
                               t_level);
}
