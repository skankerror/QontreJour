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

#include "dmxchannelgroup.h"
#include <QDebug>

DmxChannelGroup::DmxChannelGroup(QObject *parent)
  : QObject(parent)
{}

void DmxChannelGroup::setLevel(int t_level)
{
  if (m_level == t_level)
    return;
  m_level = t_level;

  emit levelChanged(m_level); // ?
}

bool DmxChannelGroup::addDmxChannel(QPair<DmxChannel *, int> t_P_dmxChannel)
{
//  auto dmxChannel = t_P_dmxChannel.first
  if (!(t_P_dmxChannel.first) || t_P_dmxChannel.second < 1)
    return false;
  if (m_L_P_dmxChannel.contains(t_P_dmxChannel))
    return setDmxChannelLevel(t_P_dmxChannel);

  m_L_P_dmxChannel.append(t_P_dmxChannel);
  return true;
}

bool DmxChannelGroup::removeDmxChannel(DmxChannel *t_dmxChannel)
{
  for (const auto &item : std::as_const(m_L_P_dmxChannel))
  {
    auto dmxChannel = item.first;
    if (dmxChannel == t_dmxChannel)
    {
      m_L_P_dmxChannel.removeOne(item);
      return true;
    }
  }
  qWarning () << "Can't remove channel, it's not in the group";
  return false;
}

bool DmxChannelGroup::setDmxChannelLevel(QPair<DmxChannel *, int> t_P_dmxChannel)
{
  if (!t_P_dmxChannel.first) return false;
  for (auto &item : m_L_P_dmxChannel)
  {
    auto dmxChannel = item.first;
    if (t_P_dmxChannel.first == dmxChannel) //
    {
      item.second = t_P_dmxChannel.second;
//      dmxChannel->setLevel(t_P_dmxChannel.second);
      return true;
    }
  }
    qWarning () << "Can't find channel to edit, it's not in the group";
    return false;
}

void DmxChannelGroup::clear()
{
  m_L_P_dmxChannel.clear();
  m_L_P_dmxChannel.squeeze();
}

void DmxChannelGroup::updateLevel(int t_level)
{
  if ((m_level == t_level)
      || (t_level < 0)
      || (t_level > 255))
    return;

  m_level = t_level;

  for (const auto &item : std::as_const(m_L_P_dmxChannel))
  {
    auto dmxChannel = item.first;
    auto level = item.second;
    double coef = double(t_level)/255.0f;
    qDebug() << "coef : "
             << coef
             << " (int)coef * level) : "
             << (int)(coef * level);
    dmxChannel->setLevel((int)(coef * level));
  }

//  emit levelChanged(m_level);
}
