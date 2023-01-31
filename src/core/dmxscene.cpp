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

int DmxScene::getStepNumber() const
{
  if(m_parentSCene)
    return m_parentSCene->m_L_subScene.indexOf(const_cast<DmxScene *>(this));
  return 0;
}

DmxScene *DmxScene::getSubscene(int t_number)
{
  if (t_number < 0
      || t_number >= m_L_subScene.size())
    return nullptr;
  return m_L_subScene.at(t_number);

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

bool DmxScene::insertNewScene(int t_position)
{
  if (t_position < 0
      || t_position > m_L_subScene.size())
    return false;
  auto string = QString("SubScene%1").arg(t_position);
  auto scene = new DmxScene(t_position,
                            string);

  if (m_type == DmxScene::RootScene)
    scene->setType(DmxScene::MainScene);
  else if (m_type == DmxScene::MainScene)
    scene->setType(DmxScene::SubScene);
  else
  {
    scene->deleteLater();
    return false;
  }
  m_L_subScene.insert(t_position, scene);
  return true;
}

bool DmxScene::insertScene(int t_position,
                           DmxScene *t_scene)
{
  if (t_position < 0
      || t_position > m_L_subScene.size())
    return false;

  m_L_subScene.insert(t_position, t_scene);
  return true;

}

bool DmxScene::removeScene(int t_position,
                           int t_count)
{
  if (t_position < 0 || t_position + t_count > m_L_subScene.size())
      return false;

  for (int row = 0; row < t_count; ++row)
      delete m_L_subScene.takeAt(t_position); // WARNING : delete is not good

  return true;

}

bool DmxScene::addScene(DmxScene *t_scene)
{
  // TODO : vérifier qu'elle n'existe pas
  if (t_scene)
  {
    m_L_subScene.append(t_scene);
    return true;
  }
}
