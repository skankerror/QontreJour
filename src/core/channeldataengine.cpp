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

#include "channeldataengine.h"
#include <QDebug>

/******************************* ChannelDataEngine ***********************/

ChannelDataEngine::ChannelDataEngine(QObject *parent)
    : QObject{parent}
{
  createDatas();
}

ChannelData *ChannelDataEngine::getChannelData(const id &t_id) const
{
  if (t_id < 0
      || t_id >= m_L_channelData.size())
    return nullptr;
  else
    return m_L_channelData.at(t_id);
}

ChannelDataEngine::~ChannelDataEngine()
{
  for (const auto &item
       : std::as_const(m_L_channelData))
  {
    item->deleteLater();
  }
  m_L_channelData.clear();
  m_L_channelData.squeeze();
}

QList<ChannelData *> ChannelDataEngine::getL_directChannelData()
{
  QList<ChannelData *> L_cd;
  for (qsizetype i = 0;
       i < m_L_directChannelId.size();
       i++)
  {
    auto cd = getChannelData(m_L_directChannelId.at(i));
    L_cd.append(cd);
  }
  return L_cd;
}

void ChannelDataEngine::addL_idToL_direct(const QList<id> &t_L_id)
{
  for (qsizetype i = 0;
       i < t_L_id.size();
       i++)
  {
    addIdToL_direct(t_L_id.at(i));
  }
}

void ChannelDataEngine::addIdToL_direct(const id &t_id)
{
  auto index = m_L_directChannelId.indexOf(t_id);
  if (index = -1)
  {
    m_L_directChannelId.append(t_id);
    getChannelData(t_id)->setIsDirectChannel(true);
  }
}

void ChannelDataEngine::removeL_idFromL_direct(const QList<id> &t_L_id)
{
  for (qsizetype i = 0;
       i < t_L_id.size();
       i++)
  {
    removeIdFromL_direct(t_L_id.at(i));
  }
}

void ChannelDataEngine::removeIdFromL_direct(const id &t_id)
{
  auto index = m_L_directChannelId.indexOf(t_id);
  if (index != -1)
  {
    m_L_directChannelId.remove(index);
    getChannelData(t_id)->setIsDirectChannel(false);
  }
}

QList<ChannelData *> ChannelDataEngine::getL_selectedChannelData()
{
  QList<ChannelData *> L_cd;
  for (qsizetype i = 0;
       i < m_L_selectedChannelId.size();
       i++)
  {
    auto cd = getChannelData(m_L_selectedChannelId.at(i));
    L_cd.append(cd);
  }
  return L_cd;
}

void ChannelDataEngine::addL_idToL_select(const QList<id> &t_L_id)
{
  for (qsizetype i = 0;
       i < t_L_id.size();
       i++)
  {
    auto selectId = t_L_id.at(i);
    addIdToL_select(selectId);
  }
}

void ChannelDataEngine::addIdToL_select(const id &t_id)
{
  auto index = m_L_selectedChannelId.indexOf(t_id);
  if (index = -1)
  {
    m_L_channelData.at(t_id)->setIsSelected(true);
    m_L_selectedChannelId.append(t_id);
  }
  emit selectionChanged(m_L_selectedChannelId);
}

void ChannelDataEngine::removeL_idFromL_select(const QList<id> &t_L_id)
{
  for (qsizetype i = 0;
       i < t_L_id.size();
       i++)
  {
    auto selectId = t_L_id.at(i);
    removeIdFromL_select(selectId);
  }
}

void ChannelDataEngine::removeIdFromL_select(const id &t_id)
{
  auto index = m_L_selectedChannelId.indexOf(t_id);
  if (index != -1)
  {
    m_L_directChannelId.remove(index);
    m_L_channelData.at(t_id)->setIsSelected(false);
  }
  emit selectionChanged(m_L_selectedChannelId);
}

void ChannelDataEngine::selectNonNullChannels()
{
  clearSelection();
  for (const auto &item
       : std::as_const(m_L_channelData))
  {
    dmx level = item->getActual_Level();
    ChannelDataFlag flag = item->getFlag();
    if (level > 0
        && (flag == ChannelDataFlag::SelectedSceneFlag
            || flag == ChannelDataFlag::DirectChannelFlag
            || flag == ChannelDataFlag::ChannelGroupFlag))
    {
      id channelId = item->getChannelID();
      addIdToL_select(channelId);
      item->setIsSelected(true);
    }
  }
//  emit selectionChanged(m_L_selectedChannelId);
}

void ChannelDataEngine::clearSelection()
{
  for (qsizetype i = 0;
       i < m_L_selectedChannelId.size();
       i++)
  {
    auto channelData = m_L_channelData.at(m_L_selectedChannelId.at(i));
    channelData->setIsSelected(false);
  }
  m_L_selectedChannelId.clear();
  m_L_selectedChannelId.squeeze();
  emit selectionChanged(m_L_selectedChannelId);
}

void ChannelDataEngine::createDatas(int t_channelCount)
{
  {
    for (int i = 0;
         i < t_channelCount;
         i++)
    {
      auto channelData = new ChannelData(i);
      m_L_channelData.append(channelData);
    }
  }
}

/******************************* ChannelData ***************************/

void ChannelData::update()
{
  setFlag(ChannelDataFlag::UnknownFlag);
  if (m_channelGroupLevel >= m_directChannelLevel)
  {
    m_actual_Level = m_channelGroupLevel;
    if (m_actual_Level)
      setFlag(ChannelDataFlag::ChannelGroupFlag);
    if (m_isDirectChannel) return;// on s'en fout de scene level
    if (m_sceneLevel >= m_actual_Level)
    {
      m_actual_Level = m_sceneLevel;
      if (m_actual_Level)
        setFlag(ChannelDataFlag::SelectedSceneFlag);
    }
  }
  else
  {
    m_actual_Level = m_directChannelLevel;
    setFlag(ChannelDataFlag::DirectChannelFlag);
    if (m_isDirectChannel) return;// on s'en fout de scene level
    if (m_sceneLevel >= m_actual_Level)
    {
      m_actual_Level = m_sceneLevel;
      setFlag(ChannelDataFlag::SelectedSceneFlag);
    }
  }
  // TODO : développer, gérer l'offset,etc...
}
