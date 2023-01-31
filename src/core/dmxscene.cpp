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


DmxScene::DmxScene(ValueType t_type,
                   DmxScene *t_parent)
  : DmxValue(t_type,
             t_parent)
{}

DmxScene::~DmxScene()
{}

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

bool DmxScene::insertNewScene(int t_position)
{
  if (t_position < 0
      || t_position > m_L_subScene.size())
    return false;
  auto string = QString("SubScene%1").arg(t_position);
  auto scene = new DmxScene(/*t_position,
                            string*/);
  scene->setID(t_position); // WARNING : CHANGE THIS
  scene->setName(string);
  scene->setParentSCene(this);
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
