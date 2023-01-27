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
                   QObject *parent)
  : QObject(parent),
    m_ID(t_ID),
    m_level(0)
{}

DmxValue::DmxValue(int t_ID,
                   QString &t_name,
                   QObject *parent)
  : QObject(parent),
    m_ID(t_ID),
    m_name(t_name),
    m_level(0)
{}

DmxValue::DmxValue(int t_universeID,
                   int t_ID,
                   QObject *parent)

  : QObject(parent),
  m_universeID(t_universeID),
  m_ID(t_ID),
  m_level(0)
{}

DmxValue::DmxValue(int t_universeID,
                   int t_ID,
                   QString &t_name,
                   QObject *parent)
  : QObject(parent),
  m_universeID(t_universeID),
  m_ID(t_ID),
  m_name(t_name),
  m_level(0)
{}

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
  emit maxLevelChanged(m_ID,
                       m_maxLevel);
}

void DmxValue::setLevel(int t_ID,
                        quint8 t_level)
{
  Q_UNUSED(t_ID)
  if (m_level == t_level
      || (t_level < 0)
      || (t_level > 255))
    return;

  m_level = t_level;

  emit levelChanged(m_ID,
                    m_level);
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

  emit levelChanged(m_ID,
                    m_level);

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
