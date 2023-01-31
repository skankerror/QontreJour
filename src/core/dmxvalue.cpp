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
#include <QDebug>


DmxValue::DmxValue(ValueType t_type,
                   DmxValue *t_parent)
  : QObject(t_parent),
    m_type(t_type)
{
  switch(m_type)
  {
  case Output :
    m_level = 0;
    m_maxLevel = 255;
    break;
  case Channel :
    m_level = 0;
    m_directChannelEditLevel = 0;
    m_channelGroupLevel = 0;
    m_selectedSceneLevel = 0;
    m_nextSceneLevel = 0;
    m_isDirectChannelEdited = false;
    break;
  case ChannelGroup :
    m_level = 0;
    m_universeID = -1;
    break;
  case Scene :
    m_level = 0;
    m_universeID = -1;
    break;
  case UnknownValueType : default :
    break;
  }
}

DmxValue::~DmxValue()
{
  DmxValue::clearList();
}

quint8 DmxValue::getChildLevel(int t_index)
{
  auto value = getChild(t_index);
  if (value) return value->getLevel();
  return -1;
}

void DmxValue::setLevel(SignalSenderType t_senderType,
                        quint8 t_level)
{
  if ((t_level < 0)
      || (t_level > 255))
    return;

  switch(m_type)
  {
  case Channel : return setChannelLevel(t_senderType, t_level); break;
  case Output : return setOutputLevel(t_senderType, t_level); break;
  case ChannelGroup : return setChannelGroupLevel(t_senderType, t_level); break;
  default :   // TODO, implement other type
    if (m_level == t_level
        || (t_level < 0)
        || (t_level > 255))
      return;
    break;
  }

}

void DmxValue::setChannelLevel(SignalSenderType t_senderType,
                               quint8 t_level)
{
  switch(t_senderType)
  {
  case ChannelGroupSender :
    if (m_channelGroupLevel == t_level)
      return;
    m_channelGroupLevel = t_level;
    break;
  case DirectChannelEditSender :
    if (m_directChannelEditLevel == t_level)
      return;
    m_directChannelEditLevel = t_level;
    m_isDirectChannelEdited = true;
    break;
  case SelectedSceneSender :
    if (m_selectedSceneLevel == t_level)
      return;
    m_selectedSceneLevel = t_level;
    break;
  case NextSceneSender :
    if (m_nextSceneLevel == t_level)
      return;
    m_nextSceneLevel = t_level;
    break;
  case UnknownSender : default :
    break;
  }

  // here we determine if output level change
  int level; // final set
  if (m_isDirectChannelEdited)
  {
    if(m_directChannelEditLevel > m_channelGroupLevel)
    {
      level = m_directChannelEditLevel;
      m_flag = DirectChannelFlag;
    }
    else
    {
      level = m_channelGroupLevel;
      m_flag = ChannelGroupFlag;
      emit blockDirectChannelSlider(level);
    }
  }
  else
  {
    if(m_channelGroupLevel > m_selectedSceneLevel)
    {
      level = m_channelGroupLevel;
      m_flag = ChannelGroupFlag;
      emit blockDirectChannelSlider(level);
    }
    else
    {
      level = m_selectedSceneLevel;
      m_flag = SelectedSceneFlag;
    }
  }
  if (m_level == level
      || (level < 0)
      || (level > 255))
    return;

  m_level = level;

  // NOTE : je sais pas en cas de gm à 0 ?
  // faut-il garder les couleurs même à 0 ?
  if (m_level == 0) m_flag = UnknownFlag;

  emit levelChanged(ChannelToOutputSender,
                    m_level);
  return;
}

void DmxValue::setOutputLevel(SignalSenderType t_senderType,
                              quint8 t_level)
{
  if (m_flag == ParkedFlag)
    return;
  if ((m_flag == IndependantFlag)
      && (t_senderType != IndependantSender))
    return;

  double coef;
  if (m_maxLevel == 0)
    coef = 0;
  else
    coef = double(m_maxLevel)/255.0f;

  quint8 level = (quint8)(coef * t_level);

  if (m_level == level
      || (level < 0)
      || (level > 255))
    return;

  m_level = level;

  emit requestDmxUpdate(m_ID,
                        m_level);

}

void DmxValue::setChannelGroupLevel(SignalSenderType t_senderType,
                                    quint8 t_level)
{
  if ((t_senderType == SubmasterSliderSender)
      || (t_senderType == SelectedSceneSender)
      || (t_senderType == NextSceneSender))
  {
    if ((m_level == t_level)
        || (t_level < 0)
        || (t_level > 255))
      return;

    m_level = t_level;

    for (int i = 0;
         (i < m_L_dmxValue.size())
         || (i < m_L_storedLevels.size());
         i++)
    {
      auto dmxChannel = m_L_dmxValue.at(i);
      auto level = m_L_storedLevels.at(i);
      double coef = double(m_level)/255.0f;
      if (t_senderType != NextSceneSender)
        dmxChannel->setLevel(DmxValue::ChannelGroupSender,
                             (quint8)(coef * level));
    }
  }

}

DmxValue *DmxValue::getChild(int t_index)
{
  if (t_index && (t_index < m_L_dmxValue.size()))
    return m_L_dmxValue.at(t_index);
  else
    return nullptr;
}

void DmxValue::setMaxLevel(quint8 t_maxLevel)
{
  if (m_maxLevel == t_maxLevel)
    return;
  m_maxLevel = t_maxLevel;
//  emit maxLevelChanged(/*m_ID,*/
//                       m_maxLevel);
}

void DmxValue::setDirectChannelEditLevel(quint8 t_directChannelEditLevel)
{
  if (m_directChannelEditLevel == t_directChannelEditLevel)
    return;
  m_directChannelEditLevel = t_directChannelEditLevel;
  emit directChannelEditLevelChanged(m_directChannelEditLevel);
}

void DmxValue::setChannelGroupLevel(quint8 t_channelGroupLevel)
{
  if (m_channelGroupLevel == t_channelGroupLevel)
    return;
  m_channelGroupLevel = t_channelGroupLevel;
  emit channelGroupLevelChanged(m_channelGroupLevel);
}

void DmxValue::setSelectedSceneLevel(quint8 t_selectedSceneLevel)
{
  if (m_selectedSceneLevel == t_selectedSceneLevel)
    return;
  m_selectedSceneLevel = t_selectedSceneLevel;
  emit selectedSceneLevelChanged(m_selectedSceneLevel);
}

void DmxValue::setNextSceneLevel(quint8 t_nextSceneLevel)
{
  if (m_nextSceneLevel == t_nextSceneLevel)
    return;
  m_nextSceneLevel = t_nextSceneLevel;
  emit nextSceneLevelChanged(m_nextSceneLevel);
}

void DmxValue::setChildLevel(SignalSenderType t_senderType,
                             int t_index,
                             quint8 t_level)
{
  if (t_index && (t_index < m_L_dmxValue.size()))
  m_L_dmxValue.at(t_index)->setLevel(t_senderType,
                                     t_level);
}

void DmxValue::setStoredLevel(int t_childIndex,
                              quint8 t_level)
{
  if (t_childIndex < 0
      || t_childIndex >= m_L_storedLevels.size())
  {
    qDebug() << "faute dans setStoredLevel ";
    return;
  }
  m_L_storedLevels[t_childIndex] = t_level;
}

void DmxValue::setID(int t_ID)
{
  if (m_ID == t_ID)
    return;
  m_ID = t_ID;
}

void DmxValue::setPropertyLevel(int t_level)
{
  if (m_level == t_level
      || (t_level < 0)
      || (t_level > 255))
    return;

  m_level = t_level;
  // TODO : implement that
//  emit levelChanged(/*m_ID,*/
//                    m_level);

}

void DmxValue::addChild(DmxValue *t_dmxValue)
{
  // we check if the pointer isn't null and if the output is not in the list.
  if(t_dmxValue && !m_L_dmxValue.contains(t_dmxValue))
  {
    m_L_dmxValue.append(t_dmxValue);
    quint8 storedLevel = 0;
    m_L_storedLevels.append(storedLevel);
    emit L_dmxValueChanged();
  }
  else
    qWarning() << "cant add value";

}

void DmxValue::addChildren(const QList<DmxValue *> t_L_dmxValue)
{
  for (const auto &item : std::as_const(t_L_dmxValue))
  {
    addChild(item);
  }
}

bool DmxValue::removeChild(const DmxValue *t_dmxValue)
{
  // we check if the pointer isn't null ...
  if(t_dmxValue)
  {
    int index = m_L_dmxValue.indexOf(t_dmxValue);
//    and if the output is in the list.
    if(index)
    {
      m_L_dmxValue.removeAt(index);
      m_L_storedLevels.removeAt(index);
      emit L_dmxValueChanged();
      //  m_L_dmxValue.squeeze(); // this freeis unused memory
      return true;
    }
  }
  qWarning() << "pointer is null or output is not in the list."
           << "can't remove value";
  return false;
}

bool DmxValue::removeChildren(const QList<DmxValue *> t_L_dmxValue)
{
  bool test;
  bool ret = true;
  for (const auto &item : std::as_const(t_L_dmxValue))
  {
    test = removeChild(item);
    if (!test)
    {
      ret = false;
    }
  }
  return ret;
}

void DmxValue::clearList()
{
  m_L_dmxValue.clear();
  m_L_storedLevels.clear();
  // emit signal ?
  m_L_dmxValue.squeeze();
  m_L_storedLevels.squeeze();
  // NOTE : do not destroy value args
  // universe or ? have to do it.
}

