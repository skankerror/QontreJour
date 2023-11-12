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
              t_parent),
    IdedValue()
{
  auto scene0 = new DmxScene(ValueType::Scene0,
                             this);
  scene0->setNotes("Blank");
  scene0->setName("0");
  scene0->setSceneID(0.0f);
  scene0->setStepNumber(0);
  m_L_childScene.append(scene0);

//  emit seqSignalChanged();
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

id Sequence::getSelectedStepId() const
{
//  return m_selectedStepId;
  for (qsizetype i = 0;
       i < m_L_childScene.size();
       i++)
  {
    auto scene = m_L_childScene.at(i);
    if (scene->getSceneID() == m_selectedSceneId)
      return i;
  }
  return 0;
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
  id size = getSize();
  t_scene->setStepNumber(size);
  m_L_childScene.append(t_scene);
  // we set to 0 selected scene
  auto scene = getScene(m_selectedSceneId);
  if (scene) scene->setLevel(NULL_DMX);
  auto newScene = getScene(size);
  // we set to 255 new scene
  if (newScene)
  {
    newScene->setLevel(MAX_DMX);
    m_selectedSceneId = newScene->getSceneID();
  }
  emit seqSignalChanged(getSelectedStepId());
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
        // la scene d'avant est pas détruite
        qWarning() << "erase scene" << t_id;
        t_scene->setStepNumber(scene->getStepNumber());
        m_L_childScene[i] = t_scene;
        t_scene->setLevel(NULL_DMX);
        t_scene->setLevel(MAX_DMX);
        emit seqSignalChanged(getSelectedStepId());
        return;
      }
      else if (scene->getSceneID() > t_id)
      {
        // we set to 0 selected scene
        auto scene = getScene(m_selectedSceneId);
        if (scene) scene->setLevel(NULL_DMX);
        // we set to 255 new scene
        t_scene->setLevel(MAX_DMX);
        m_selectedSceneId = t_scene->getSceneID();
        m_L_childScene.insert(i, t_scene);
        update(i);
        emit seqSignalChanged(getSelectedStepId());
        return;
      }
    }
    // we're at the end, scen id is the highest of the seq
    t_scene->setStepNumber(m_L_childScene.size());
    // we set to 0 selected scene
    auto scene = getScene(m_selectedSceneId);
    if (scene) scene->setLevel(NULL_DMX);
    // we set to 255 new scene
    t_scene->setLevel(MAX_DMX);
    m_selectedSceneId = t_scene->getSceneID();
    m_L_childScene.append(t_scene);
    emit seqSignalChanged(getSelectedStepId());
    return;
  }
  // TODO : ça va pas
  emit seqSignalChanged(getSelectedStepId());
}

void Sequence::removeScene(id t_step)
{
  // TODO :
  emit seqSignalChanged(getSelectedStepId());
}

void Sequence::removeScene(sceneID_f)
{
  // TODO :

  emit seqSignalChanged(getSelectedStepId());

}

void Sequence::setSelectedStepId(id t_selectedStepId)
{
  if (t_selectedStepId < m_L_childScene.size()
      && t_selectedStepId >= 0)
  {
    // we set to 0 selected scene
    auto scene = getScene(m_selectedSceneId);
    if (scene) scene->setLevel(NULL_DMX);
    auto newScene = getScene(t_selectedStepId);
    // we set to 255 new scene
    if (newScene)
    {
      newScene->setLevel(MAX_DMX);
      m_selectedSceneId = newScene->getSceneID();
    }
    emit seqSignalChanged(getSelectedStepId());
  }
  else
    qDebug() << "problem in cue selection";
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
void DmxScene::setLevel(dmx t_level)
{
  m_level = t_level;
  emit sceneLevelChanged(m_sceneID,
                         m_level);
}

/*************************** DmxSubScene *******************************/

SubScene::SubScene(ValueType t_type,
                   DmxScene *t_parent)
    : DmxScene(t_type,
               t_parent->getSequence())
{
  setParentScene(t_parent);
}


