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

#ifndef CHANNELDATAENGINE_H
#define CHANNELDATAENGINE_H

#include <QObject>
#include "../qontrejour.h"


class ChannelData;

class ChannelDataEngine
    : public QObject
{
  Q_OBJECT

public :

  explicit ChannelDataEngine(QObject *parent = nullptr);

  ChannelData *getChannelData(const id &t_id) const;

  ~ChannelDataEngine();

  QList<id> getL_directChannelId() const
  { return m_L_directChannelId; }
  QList<ChannelData *> getL_directChannelData();
  void addL_idToL_direct(const QList<id> &t_L_id);
  void addIdToL_direct(const id &t_id);
  void removeL_idFromL_direct(const QList<id> &t_L_id);
  void removeIdFromL_direct(const id &t_id);

  QList<id> getL_selectedChannelId() const
  { return m_L_selectedChannelId; }
  QList<ChannelData *> getL_selectedChannelData();
  void addL_idToL_select(const QList<id> &t_L_id);
  void addIdToL_select(const id &t_id);
  void removeL_idFromL_select(const QList<id> &t_L_id);
  void removeIdFromL_select(const id &t_id);

  qsizetype getChannelDataCount()
  { return m_L_channelData.size(); }

  void selectNonNullChannels();
  void clearSelection();
  void clearDirectChannel();

private :

  void createDatas(int t_channelCount = DEFAULT_CHANNEL_COUNT);

signals :

  void sigToUpdateChannelView();

private :

  QList<ChannelData *> m_L_channelData;
  QList<id> m_L_directChannelId;
  QList<id> m_L_selectedChannelId;
};

/******************************* ChannelData ***************************/

class ChannelData
    : public QObject
{

  Q_OBJECT

public :

  explicit ChannelData(id t_id = NO_ID,
                       dmx t_channelGroupLevel = NULL_DMX,
                       dmx t_directChannelLevel = NULL_DMX,
                       overdmx t_directChannelOffset = NULL_DMX_OFFSET,
                       dmx t_sceneLevel = NULL_DMX,
                       dmx t_nextSceneLevel = NULL_DMX,
                       QObject *parent = nullptr)
      : QObject(parent),
      m_channelID(t_id),
      m_channelGroupLevel(t_channelGroupLevel),
      m_directChannelLevel(t_directChannelLevel),
      m_directChannelOffset(t_directChannelOffset),
      m_sceneLevel(t_sceneLevel),
      m_nextSceneLevel(t_nextSceneLevel)
  {}

  ChannelData(const ChannelData &channel)
      : QObject(channel.parent())
  {
    m_channelID = channel.getChannelID();
    m_channelGroupLevel = channel.getChannelGroupLevel();
    m_directChannelLevel = channel.getDirectChannelLevel();
    m_directChannelOffset = channel.getDirectChannelOffset();
    m_sceneLevel = channel.getSceneLevel();
    m_nextSceneLevel = channel.getNextSceneLevel();
    m_flag = channel.getFlag();
    m_actual_Level = channel.getActual_Level();
    m_isDirectChannel = channel.getIsDirectChannel();
    m_isSelected = channel.getIsSelected();
  }

  ~ChannelData(){}

  dmx getChannelGroupLevel() const{ return m_channelGroupLevel; }
  dmx getDirectChannelLevel() const{ return m_directChannelLevel; }
  overdmx getDirectChannelOffset() const{ return m_directChannelOffset; }
  dmx getSceneLevel() const{ return m_sceneLevel; }
  dmx getNextSceneLevel() const{ return m_nextSceneLevel; }
  dmx getActual_Level() const{ return m_actual_Level; }
  ChannelDataFlag getFlag() const{ return m_flag; }
  id getChannelID() const{ return m_channelID; }
  bool getIsSelected() const{ return m_isSelected; }
  bool getIsDirectChannel() const{ return m_isDirectChannel; }

  void setChannelGroupLevel(dmx t_channelGroupLevel)
  { m_channelGroupLevel = t_channelGroupLevel; }
  void setDirectChannelLevel(dmx t_directChannelLevel)
  { m_directChannelLevel = t_directChannelLevel; }
  void setDirectChannelOffset(overdmx t_directChannelOffset)
  { m_directChannelOffset = t_directChannelOffset; }
  void setSceneLevel(dmx t_sceneLevel){ m_sceneLevel = t_sceneLevel; update(); }
  void setNextSceneLevel(dmx t_nextSceneLevel)
  { m_nextSceneLevel = t_nextSceneLevel; }
  void setChannelID(id t_channelID){ m_channelID = t_channelID; }
  void setActual_Level(dmx t_actual_Level){ m_actual_Level = t_actual_Level; }
  void setIsSelected(bool t_isSelected)
  { m_isSelected = t_isSelected;
    if (!t_isSelected) clearOverdmx(); }
  void setIsDirectChannel(bool t_isDirectChannel)
  { m_isDirectChannel = t_isDirectChannel; }

  void clearChannel()
  {
    m_directChannelLevel = NULL_DMX;
    m_directChannelOffset = NULL_DMX_OFFSET;
    m_sceneLevel = NULL_DMX;
    m_nextSceneLevel = NULL_DMX;
    // TODO : gerer le flag
  }

  void clearDirectChannel()
  {
    m_directChannelLevel = NULL_DMX;
    m_directChannelOffset = NULL_DMX_OFFSET;
    m_isDirectChannel = false;
    //    m_sceneLevel = NULL_DMX;
  }

  void clearOverdmx(){ m_directChannelOffset = NULL_DMX_OFFSET; }
  void update();

private :

  void setFlag(ChannelDataFlag t_flag){ m_flag = t_flag; }

  Q_PROPERTY(dmx sceneLevel
                 READ getSceneLevel
                     WRITE setSceneLevel
/*                         RESET resetLevel
                             NOTIFY levelChanged
                                 FINAL*/)


private :

  id m_channelID = NO_ID;
  dmx m_channelGroupLevel = NULL_DMX;
  dmx m_directChannelLevel = NULL_DMX;
  overdmx m_directChannelOffset = NULL_DMX_OFFSET;
  dmx m_sceneLevel = NULL_DMX;
  dmx m_nextSceneLevel = NULL_DMX;
  ChannelDataFlag m_flag = UnknownFlag;

  dmx m_actual_Level = NULL_DMX;
  bool m_isSelected = false;
  bool m_isDirectChannel = false;
};
Q_DECLARE_METATYPE(ChannelData)

#endif // CHANNELDATAENGINE_H
