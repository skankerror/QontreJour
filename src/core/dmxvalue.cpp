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

DmxValue::DmxValue(int t_ID,
                   QObject *parent,
                   ValueType t_type)
  : QObject(parent),
    m_ID(t_ID),
    m_type(t_type),
    m_level(0)
{
  constructorMethod();
}

DmxValue::DmxValue(int t_ID,
                   QString &t_name,
                   QObject *parent,
                   ValueType t_type)
  : QObject(parent),
    m_ID(t_ID),
    m_name(t_name),
    m_type(t_type),
    m_level(0)
{
  constructorMethod();
}

DmxValue::DmxValue(int t_universeID,
                   int t_ID,
                   QObject *parent,
                   ValueType t_type)
  : QObject(parent),
  m_universeID(t_universeID),
  m_ID(t_ID),
  m_type(t_type),
  m_level(0)
{
  constructorMethod();
}

DmxValue::DmxValue(int t_universeID,
                   int t_ID,
                   QString &t_name,
                   QObject *parent,
                   ValueType t_type)
  : QObject(parent),
  m_universeID(t_universeID),
  m_ID(t_ID),
  m_name(t_name),
  m_type(t_type),
  m_level(0)
{}

void DmxValue::constructorMethod()
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
case Unknown : default :
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
    }
  }
  else
  {
    if(m_channelGroupLevel > m_selectedSceneLevel)
    {
      level = m_channelGroupLevel;
      m_flag = ChannelGroupFlag;
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

DmxValue::~DmxValue()
{
  DmxValue::clearList();
}

DmxValue *DmxValue::getL_dmxValueAt(int t_index)
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

void DmxValue::setLevel(SignalSenderType t_senderType,
                        quint8 t_level)
{
  if ((t_level < 0)
      || (t_level > 255))
    return;

  if (m_type == Channel)
  {
    return setChannelLevel(t_senderType,
                           t_level);
  }
  if (m_type == Output)
  {
    return setOutputLevel(t_senderType,
                          t_level);
  }
  // TODO, implement other type

  if (m_level == t_level
      || (t_level < 0)
      || (t_level > 255))
    return;

//  m_level = t_level;

//  emit levelChanged(m_type,
//                    m_level);


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

void DmxValue::addDmxValue(DmxValue *t_dmxValue)
{
  // we check if the pointer isn't null and if the output is not in the list.
  if(t_dmxValue && !m_L_dmxValue.contains(t_dmxValue))
  {
    m_L_dmxValue.append(t_dmxValue);
    emit L_dmxValueChanged();
  }
  else
    qWarning() << "cant add value";

}

void DmxValue::addDmxValues(const QList<DmxValue *> t_L_dmxValue)
{
  for (const auto &item : std::as_const(t_L_dmxValue))
  {
    addDmxValue(item);
  }

}

bool DmxValue::removeDmxValue(const DmxValue *t_dmxValue)
{
  // we check if the pointer isn't null ...
  if(t_dmxValue)
  {
    int index = m_L_dmxValue.indexOf(t_dmxValue);
//    and if the output is in the list.
    if(index)
    {
      m_L_dmxValue.removeAt(index);
      emit L_dmxValueChanged();
      //  m_L_dmxValue.squeeze(); // this freeis unused memory
      return true;
    }
  }
  qWarning() << "pointer is null or output is not in the list."
           << "can't remove value";
  return false;


}

bool DmxValue::removeDmxValues(const QList<DmxValue *> t_L_dmxValue)
{
  // on peut utiliser aussi QList::removeAll()
  bool test;
  bool ret = true;
  for (const auto &item : std::as_const(t_L_dmxValue))
  {
    test = removeDmxValue(item);
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
  // emit signal ?
  m_L_dmxValue.squeeze();
  // NOTE : do not destroy value args
  // universe or ? have to do it.

}

