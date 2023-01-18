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

#include "dmxchannel.h"
#include <QDebug>

DmxChannel::DmxChannel(int t_universeID,
                       int t_channelID,
                       QObject *parent)
  : QObject(parent),
    m_level(0),
    m_universeID(t_universeID),
    m_channelID(t_channelID)
{}

DmxChannel::~DmxChannel()
{
  DmxChannel::clear();
}

DmxOutput *DmxChannel::getL_dmxOutputAt(int t_index)
{
  if (t_index && t_index < m_L_dmxOutput.size())
    return m_L_dmxOutput.at(t_index);
  else
    return nullptr;
}

void DmxChannel::setLevel(const int t_level)
{
  if (m_level == t_level)
    return;
  m_level = t_level;
  emit levelChanged(m_channelID,
                    m_level);
}

void DmxChannel::addDmxOutput(DmxOutput *t_dmxOutput)
{
  // we check if the pointer isn't null and if the output is not in the list.
  if(t_dmxOutput && !m_L_dmxOutput.contains(t_dmxOutput))
  {
    m_L_dmxOutput.append(t_dmxOutput);
    emit L_dmxOutputChanged();
  }
  else
    qWarning() << "cant add output to channel";
}

void DmxChannel::addDmxOutputs(const QList<DmxOutput *> t_L_dmxOutput)
{
  for (const auto &item : std::as_const(t_L_dmxOutput))
  {
    addDmxOutput(item);
  }
}

bool DmxChannel::removeDmxOutput(const DmxOutput *t_dmxOutput)
{
  // we check if the pointer isn't null ...
  if(t_dmxOutput)
  {
    int index = m_L_dmxOutput.indexOf(t_dmxOutput);
//    and if the output is in the list.
    if(index)
    {
      m_L_dmxOutput.removeAt(index);
      emit L_dmxOutputChanged();
      //  m_L_dmxOutput.squeeze(); // this freeis unused memory
      return true;
    }
  }
  qDebug() << "pointer is null or output is not in the list."
           << "can't remove it from channel";
  return false;

}

bool DmxChannel::removeDmxOutputs(const QList<DmxOutput *> t_L_dmxOutput)
{
  // on peut utiliser aussi QList::removeAll()
  bool test;
  bool ret = true;
  for (const auto &item : std::as_const(t_L_dmxOutput))
  {
    test = removeDmxOutput(item);
    if (!test)
    {
      ret = false;
    }
  }
  return ret;
}

void DmxChannel::clear()
{
  m_L_dmxOutput.clear();
  // emit signal ?
  // TODO : delete the outputs ?
  m_L_dmxOutput.squeeze();
}
