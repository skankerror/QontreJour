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

#include "dmxvalue.h"
#include "dmxmanager.h"
#include <QDebug>
#include <QtMath>

/********************************* ROOTVALUE *************************************/

RootValue::RootValue(ValueType t_type,
                     DmxValue *t_parent)
  : DmxValue(t_type,
             t_parent)
{
  switch(t_type)
  {
  case ValueType::RootOutputType :
    setName("Root Output");
    break;
  case ValueType::RootChannel :
    setName("Root Channel");
    break;
  case ValueType::RootChannelGroup :
    setName("Root Channel Group");
    break;
  case ValueType::SequenceType :
    setName("Sequnce");
    break;
  default :
    break;
  }
}

RootValue::~RootValue()
{
  for (const auto &item
       : std::as_const(m_L_childValue))
  {
    item->deleteLater();
  }
  m_L_childValue.clear();
  m_L_childValue.squeeze();
}

LeveledValue *RootValue::getChildValue(id t_id) const
{
  if ((t_id > NO_ID)
      && (t_id < m_L_childValue.size()))
    return m_L_childValue.at(t_id);
  else
    return nullptr;
}

void RootValue::addChildValue(LeveledValue *t_value)
{
  // we check if the pointer isn't null and if the value is not in the list.
  if(t_value && !m_L_childValue.contains(t_value))
  {
    m_L_childValue.append(t_value);
    t_value->setParentValue(this);
  }
  else
    qWarning() << "cant add child value";
}

void RootValue::addChlidList(const QList<LeveledValue *> t_L_value)
{
  for (const auto &item:
       std::as_const(t_L_value))
  {
    addChildValue(item);
  }
}

void RootValue::removeChildValue(const id t_index)
{
  if (t_index < 0
      || t_index >= m_L_childValue.size())
  {
    auto value = m_L_childValue.at(t_index);
    value->setParentValue(nullptr);
    m_L_childValue.removeAt(t_index);
  }
  else
  {
    qWarning() << "can't remove value";
  }
}

void RootValue::removeChildValueList(const QList<id> t_L_index)
{
  for (const auto item :
       std::as_const(t_L_index))
  {
    removeChildValue(item);
  }
}

/********************************* DMXOUTPUT *************************************/

void DmxOutput::setLevel(dmx t_level)
{
  if (m_level == t_level)
    return;
  m_level = t_level;
  emit outputRequestUpdate(m_uid,
                           m_id,
                           m_level);
}

/********************************** DMXCHANNEL ************************************/

DmxOutput *DmxChannel::getControledOutput(const id t_index)
{
  if ((t_index > -1)
      && (t_index < m_L_controledOutput.size()))
    return m_L_controledOutput.at(t_index);
  else
    return nullptr;
}

dmx DmxChannel::getControledOutputLevel(const id t_index)
{
  if ((t_index > -1)
      && (t_index < m_L_controledOutput.size()))
  {
    return m_L_controledOutput.at(t_index)->getLevel();
  }
  else
  {
    qWarning() << "Problem in DmxChannel::getControledOutputLevel";
    return NULL_DMX;
  }
}

void DmxChannel::addOutput(DmxOutput *t_dmxOutput)
{
  // we check if the pointer isn't null and if the value is not in the list.
  if(t_dmxOutput
     && !m_L_controledOutput.contains(t_dmxOutput))
  {
    m_L_controledOutput.append(t_dmxOutput);
  }
  else
  {
    qWarning() << "cant add output";
  }
}

void DmxChannel::addOutput(Uid_Id t_Uid_Id)
{
  addOutput(MANAGER->getOutput(t_Uid_Id.getUniverseID(),
                               t_Uid_Id.getOutputID()));
}

void DmxChannel::addOutputList(const QList<DmxOutput *> t_L_controledOutput)
{
  for (const auto &item :
       std::as_const(t_L_controledOutput))
  {
    addOutput(item);
  }
}

void DmxChannel::removeOutput(const DmxOutput *t_output)
{
  auto index = m_L_controledOutput.indexOf(t_output);
  if (index != -1)
  {
    m_L_controledOutput.removeAt(index);
  }
  else
  {
    qWarning() << "can't DmxChannel::removeOutput";
  }
}

void DmxChannel::removeOutput(const Uid_Id t_Uid_Id)
{
  removeOutput(MANAGER->getOutput(t_Uid_Id.getUniverseID(),
                                  t_Uid_Id.getOutputID()));
}

void DmxChannel::removeOutputList(const QList<DmxOutput *> t_L_output)
{
  for (const auto &item
       : std::as_const(t_L_output))
  {
    removeOutput(item);
  }
}

void DmxChannel::removeOutputList(const QList<Uid_Id> t_L_Uid_Id)
{
  for (const auto &item
       : std::as_const(t_L_Uid_Id))
  {
    removeOutput(item);
  }
}

void DmxChannel::clearControledOutput()
{
  m_L_controledOutput.clear();
  m_L_controledOutput.squeeze();
}

/********************************** DMXCHANNELGROUP ************************************/

DmxChannelGroup::DmxChannelGroup(ValueType t_type,
                                 RootValue *t_parent)
  : LeveledValue(t_type,
                 t_parent)
{
  setName(DEFAULT_GROUP_NAME);
}

DmxChannelGroup::~DmxChannelGroup()
{
  clearControledChannel();
}

dmx DmxChannelGroup::getControledChannelStoredLevel(const id t_id)
{
  return getControledChannelStoredLevel(MANAGER->getChannel(t_id));
}

dmx DmxChannelGroup::getControledChannelStoredLevel(/*const */DmxChannel *m_channel)
{
  if (m_channel == nullptr
      || !m_H_controledChannel_storedLevel.contains(m_channel))
  {
    qWarning() << " problem in DmxChannelGroup::getControledChannelStoredLeve";
    return NULL_DMX;
  }
  return m_H_controledChannel_storedLevel.value(m_channel);
}

QList<id> DmxChannelGroup::getL_channelId()
{
  QList<id> L_return;
  for (auto i = m_H_controledChannel_storedLevel.cbegin(),
       end = m_H_controledChannel_storedLevel.cend();
       i != end; ++i)
  {
    id channelId = i.key()->getid();
    L_return.append(channelId);
  }
  return L_return;
}

// NOTE : this method modifies level if channel was already in group
void DmxChannelGroup::addChannel(DmxChannel *t_dmxChannel,
                                 const dmx t_storedLevel)
{
  // we check if the pointer isn't null.
  if(t_dmxChannel)
  {
    m_H_controledChannel_storedLevel.insert(t_dmxChannel,
                                            t_storedLevel);
  }
  else
    qWarning() << "cant DmxChannelGroup::addChannel";

}

void DmxChannelGroup::addChannel(const id t_id,
                                 const dmx t_storedLevel)
{
  auto channel = MANAGER->getChannel(t_id);
  addChannel(channel,
             t_storedLevel);
}

//void DmxChannelGroup::addChannelList(QList<DmxChannel *> t_L_controledChannel,
//                                     QList<dmx> t_L_storedLevel)
//{
//  for (int i = 0;
//       i < t_L_controledChannel.size()
//       || i < t_L_storedLevel.size();
//       i++)
//  {
//    addChannel(t_L_controledChannel.at(i),
//               t_L_storedLevel.at(i));
//  }
//}

void DmxChannelGroup::removeChannel(DmxChannel *t_channel)
{
  if (!m_H_controledChannel_storedLevel.contains(t_channel))
  {
    qWarning() << "can't DmxChannelGroup::removeChannel";
    return;
  }
  m_H_controledChannel_storedLevel.remove(t_channel);
//  t_channel->removeChannelGroupControler(m_ID);
}

void DmxChannelGroup::removeChannel(const id t_id)
{
  removeChannel(MANAGER->getChannel(t_id));
}

void DmxChannelGroup::removeChannelList(const QList<DmxChannel *> t_L_channel)
{
  for (const auto &item
       : std::as_const(t_L_channel))
  {
    removeChannel(item);
  }
}

void DmxChannelGroup::removeChannelList(const QList<id> t_L_id)
{
  for (const auto item
       : std::as_const(t_L_id))
  {
    removeChannel(item);
  }
}

void DmxChannelGroup::clearControledChannel()
{
  m_H_controledChannel_storedLevel.clear();
}

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
}

Sequence::~Sequence()
{
  m_L_childScene.clear();
  m_L_childScene.squeeze();
}

DmxScene *Sequence::getScene(id t_step)
{
  if (t_step < m_L_childScene.size()
      && t_step > -1)
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
  t_scene->setSequence(this);
  // we set to 0 selected scene
//  auto scene = getScene(m_selectedSceneId);
//  if (scene) scene->setLevel(NULL_DMX);
//  auto newScene = getScene(size);
  // we set to 255 new scene
//  if (newScene)
//  {
//    newScene->setLevel(MAX_DMX);
    m_selectedSceneId = t_scene->getSceneID();
//  }
  emit seqSignalChanged(getSelectedStepId());
}

void Sequence::addScene(DmxScene *t_scene,
                        sceneID_f t_id)
{
  // to be sure
  t_scene->setSceneID(t_id);

  // BUG : ça va pas il faut vérifier l'id plutôt
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
//        scene->setLevel(NULL_DMX);
//        t_scene->setLevel(MAX_DMX);
        t_scene->setSequence(this);
        emit seqSignalChanged(getSelectedStepId());
        return;
      }
      else if (scene->getSceneID() > t_id)
      {
        // we set to 0 selected scene
//        auto scene = getScene(m_selectedSceneId);
//        if (scene) scene->setLevel(NULL_DMX);
        // we set to 255 new scene
//        t_scene->setLevel(MAX_DMX);
        m_selectedSceneId = t_scene->getSceneID();
        m_L_childScene.insert(i, t_scene);
        t_scene->setSequence(this);
        update(i);
        emit seqSignalChanged(getSelectedStepId());
        return;
      }
    }
    // we're at the end, scen id is the highest of the seq
    t_scene->setStepNumber(m_L_childScene.size());
    // we set to 0 selected scene
//    auto scene = getScene(m_selectedSceneId);
//    if (scene) scene->setLevel(NULL_DMX);
    // we set to 255 new scene
//    t_scene->setLevel(MAX_DMX);
    m_selectedSceneId = t_scene->getSceneID();
    m_L_childScene.append(t_scene);
    t_scene->setSequence(this);
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
//    auto scene = getScene(m_selectedSceneId);
//    if (scene) scene->setLevel(NULL_DMX);
    auto newScene = getScene(t_selectedStepId);
    // we set to 255 new scene
    if (newScene)
    {
//      newScene->setLevel(MAX_DMX);
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
{
  setName(DEFAULT_SCENE_NAME);
}

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


