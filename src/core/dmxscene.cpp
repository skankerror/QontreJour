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
#include <QtMath>

/****************************** RootScene ****************************/

Sequence::Sequence(ValueType t_type,
                   DmxValue *t_parent)
  : RootValue(t_type,
              t_parent)
{
  auto scene0 = new DmxScene(ValueType::Scene0,
                             this);
  scene0->setNotes("Blank");
  scene0->setName("0");
  scene0->setSceneID(0.0f);
  scene0->setStepNumber(0);
  m_L_childScene.append(scene0);

  emit seqSizeChanged();
}

Sequence::~Sequence()
{
  m_L_childScene.clear();
  m_L_childScene.squeeze();
}

DmxScene *Sequence::getScene(id t_step)
{
  if (t_step < m_L_childScene.size())
    return m_L_childScene.at(t_step);

  return nullptr;
}

DmxScene *Sequence::getScene(sceneID_f t_id)
{
  for (qsizetype i = 0;
       i < m_L_childScene.size();
       i++)
  {
    auto scene = m_L_childScene.at(i);
    if (t_id == scene->getSceneID())
      return scene;
  }
  return nullptr;
}

void Sequence::addScene(DmxScene *t_scene)
{
  sceneID_f lastID = m_L_childScene.last()->getSceneID();
  auto intIndex = qCeil(lastID);
  if (intIndex <10)
    t_scene->setSceneID(10.0f);
  else
  {
    int newId = intIndex / 10; // on garde les dizaines
    newId++; // on ajoute 1
    newId *= 10; // on multiplie par 10
    t_scene->setSceneID(newId);
  }
  t_scene->setStepNumber(m_L_childScene.size());
  m_L_childScene.append(t_scene);
//  update(m_L_childScene.size() - 1);
  emit seqSizeChanged();
}

void Sequence::addScene(DmxScene *t_scene,
                        sceneID_f t_id)
{
  // to be sure
  t_scene->setSceneID(t_id);

  auto index = m_L_childScene.indexOf(t_scene);
  if (index == -1) // ID is not in seq
  {
    for (qsizetype i = 0;
         i < m_L_childScene.size();
         i++)
    {
      auto scene = m_L_childScene.at(i);
      if (scene->getSceneID() == t_id)
      {
        // TODO : ouvrir une fenetre pour confirmer
        qWarning() << "erase scene" << t_id;
        t_scene->setStepNumber(scene->getStepNumber());
        m_L_childScene[i] = t_scene;
        return;
      }
      if (scene->getSceneID() > t_id)
      {
        m_L_childScene.insert(i + 1, t_scene);
        update(i + 1);
      }
    }
  }
  emit seqSizeChanged();
}

void Sequence::removeScene(id t_step)
{

  emit seqSizeChanged();

}

void Sequence::removeScene(sceneID_f)
{
  emit seqSizeChanged();

}

void Sequence::update(id t_step)
{
  if (t_step >= m_L_childScene.size())
  {
    qDebug() << "problem in Sequence::update";
    return;
  }
  for (qsizetype i = t_step;
       i < m_L_childScene.size();
       i++)
  {
    auto scene = m_L_childScene.at(i);
    scene->setStepNumber(i);
  }
}

/****************************** DmxScene *****************************/

DmxScene::DmxScene(ValueType t_type,
                   Sequence *t_parent)
    : DmxChannelGroup(t_type,
                      t_parent),
    m_sequence(t_parent)
{}

DmxScene::DmxScene(DmxScene &t_scene)
    : DmxChannelGroup(t_scene.getType(),
                      t_scene.getSequence()),
    m_sequence(t_scene.getSequence())
{}

DmxScene::~DmxScene()
{}

bool DmxScene::operator <(const DmxScene &t_scene) const
{
  return getSceneID() < t_scene.getSceneID() ?
             true : false;
}

bool DmxScene::operator >(const DmxScene &t_scene) const
{
  return getSceneID() > t_scene.getSceneID() ?
             true : false;

}

bool DmxScene::operator <=(const DmxScene &t_scene) const
{
  return getSceneID() <= t_scene.getSceneID() ?
             true : false;
}

bool DmxScene::operator >=(const DmxScene &t_scene) const
{
  return getSceneID() >= t_scene.getSceneID() ?
             true : false;
}

void DmxScene::addSubScene(SubScene *t_subScene)
{
  // TODO : vérifier, lui donner une id...
  m_L_subScene.append(t_subScene);
}

/*************************** DmxSubScene *******************************/

SubScene::SubScene(ValueType t_type,
                   DmxScene *t_parent)
    : DmxScene(t_type,
               t_parent->getSequence())
{
  setParentScene(t_parent);
}
