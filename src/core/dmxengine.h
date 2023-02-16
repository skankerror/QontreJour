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

#ifndef DMXENGINE_H
#define DMXENGINE_H

#include <QObject>
#include "dmxvalue.h"
#include "../qontrejour.h"

#define ENGINE DmxEngine::instance()
#define GROUP_ENGINE ENGINE->getGroupEngine()
#define Null_Id_Dmx Id_Dmx(NO_ID,0)

/******************************* DmxEngine ***************************/

class ChannelGroupEngine;

class DmxEngine
    : public QObject
{

  Q_OBJECT

public :

  static DmxEngine *instance();

  ~DmxEngine();

  ChannelGroupEngine *getGroupEngine() const{ return m_groupEngine; }

private :

  explicit DmxEngine(QObject *parent = nullptr);

private :

  ChannelGroupEngine *m_groupEngine;

};

/****************************** ChannelGroupEngine ***********************/

class GlobalChannelGroup;

class ChannelGroupEngine :
    public QObject
{

  Q_OBJECT

public :

  explicit ChannelGroupEngine(QObject *parent = nullptr);

  ~ChannelGroupEngine();

  void newGroupCreated(DmxChannelGroup *t_newGroup);

public slots :

  void groupLevelEvent(dmx t_level);

private :

  GlobalChannelGroup *m_globalGroup;
};

/****************************** Id_Dmx ********************************/

class Id_Dmx
{

public :

  explicit Id_Dmx(const id t_ID = NO_ID,
                  const dmx t_level = Null_Dmx)
    : m_ID(t_ID),
      m_level(t_level)
  {}

  ~Id_Dmx(){}

  bool operator==(const Id_Dmx t_id_dmx) const
  { return ((t_id_dmx.getID() == m_ID)
            && (t_id_dmx.getLevel() == m_level)); }
  bool isBrother(const Id_Dmx t_id_dmx) const
  { return (m_ID == t_id_dmx.getID()); }

  // WARNING : use isBrother() before these operators
  bool operator<(const Id_Dmx t_id_dmx) const
  { return (m_level < t_id_dmx.getLevel()); }
  bool operator>(const Id_Dmx t_id_dmx) const
  { return (m_level > t_id_dmx.getLevel()); }
  bool operator<=(const Id_Dmx t_id_dmx) const
  { return (m_level <= t_id_dmx.getLevel()); }
  bool operator>=(const Id_Dmx t_id_dmx) const
  { return (m_level >= t_id_dmx.getLevel()); }


  id getID() const{ return m_ID; }
  dmx getLevel() const{ return m_level; }

  void setID(const id t_ID){ m_ID = t_ID; }
  void setLevel(const dmx t_level){ m_level = t_level; }

  bool isValid() const{ return (m_ID > NO_ID); }

private :

  id m_ID = NO_ID;
  dmx m_level = Null_Dmx;
};

/******************************* GlobalChannelGroup ********************/

class GlobalChannelGroup
{

public :

  explicit GlobalChannelGroup();

  ~GlobalChannelGroup();

  void addChannelGroup(id t_groupID,
                       QList<Id_Dmx> t_L_id_dmx);

  bool removeChannelGroup(id t_groupID);

private :

  bool addChannel(const id t_groupID,
                  const Id_Dmx t_id_dmx);

  void groupLevelChanged(const id t_groupID,
                         const dmx t_level);

private :

  // id : group id , Id_Dmx : channelId _ level
  QMultiMap<id, Id_Dmx> m_MM_totalGroup;
  // channel Id , higher Group Id _ level
  QMap<id, Id_Dmx> m_M_channelLevel;
};

#endif // DMXENGINE_H
