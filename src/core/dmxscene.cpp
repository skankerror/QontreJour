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

#include "dmxscene.h"
#include <QDebug>

DmxScene::DmxScene(int t_ID,
                   QString &t_name,
                   QObject *parent,
                   ValueType t_type)
  : DmxChannelGroup(t_ID,
                    t_name,
                    parent,
                    t_type)
{}

DmxScene::~DmxScene()
{}

DmxChannelGroup *DmxScene::fromSceneToChannelGroup()
{
  return nullptr;
// TODO : change channel group in channel
}

void DmxScene::addDmxChannelGroup(std::pair<DmxChannelGroup *, quint8> t_P_dmxChannelGroup)
{
  if (!(t_P_dmxChannelGroup.first) || t_P_dmxChannelGroup.second < 1)
    return;
  if (m_L_P_dmxChannelGroup.contains(t_P_dmxChannelGroup))
    return setDmxChannelGroupLevel(t_P_dmxChannelGroup);

  m_L_P_dmxChannel.append(t_P_dmxChannelGroup);
  return ;

}

void DmxScene::removeDmxChannelGroup(DmxChannelGroup *t_dmxChannelGroup)
{
  for (const auto &item : std::as_const(m_L_P_dmxChannelGroup))
  {
    auto dmxChannelGroup = item.first;
    if (dmxChannelGroup == t_dmxChannelGroup)
    {
      m_L_P_dmxChannelGroup.removeOne(item);
      return;
    }
  }
  qWarning () << "Can't remove channel group, it's not in the scene";
  return;

}

void DmxScene::setDmxChannelGroupLevel(std::pair<DmxChannelGroup *, quint8> t_P_dmxChannelGroup)
{
  if (!t_P_dmxChannelGroup.first) return;
  for (auto &item : m_L_P_dmxChannelGroup)
  {
    auto dmxChannelGroup = item.first;
    if (t_P_dmxChannelGroup.first == dmxChannelGroup) //
    {
      item.second = t_P_dmxChannelGroup.second;
      // NOTE : don't emit signal, this is internal
//      dmxChannel->setLevel(t_P_dmxChannel.second);
      return;
    }
  }
    qWarning () << "Can't find channel group to edit, it's not in the scene";
    return;

}
