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

#define ENGINE DmxEngine::instance()
#define GROUP_ENGINE ENGINE->getGroupEngine()
#define CHANNEL_ENGINE ENGINE->getChannelEngine()
#define NULL_CH_ID_DMX Ch_Id_Dmx(NO_ID,NULL_DMX)
#define NULL_GR_ID_DMX Gr_Id_Dmx(NO_ID,NULL_DMX)

/******************************* DmxEngine ***************************/

class ChannelGroupEngine;
class ChannelEngine;

class DmxEngine
    : public QObject
{

  Q_OBJECT

public :

  static DmxEngine *instance();

  ~DmxEngine();

  ChannelGroupEngine *getGroupEngine() const{ return m_groupEngine; }
  ChannelEngine *getChannelEngine() const{ return m_channelEngine; }

private :

  explicit DmxEngine(QObject *parent = nullptr);

private :

  ChannelGroupEngine *m_groupEngine;
  ChannelEngine *m_channelEngine;

};

/****************************** ChannelGroupEngine ***********************/

class Ch_Id_Dmx;
class Gr_Id_Dmx;

class ChannelGroupEngine :
    public QObject
{

  Q_OBJECT

public :

  explicit ChannelGroupEngine(QObject *parent = nullptr);

  ~ChannelGroupEngine();

  bool addNewGroup(const DmxChannelGroup *t_newGroup);
  bool addNewGroup(const id t_groupId);
  bool removeGroup(const DmxChannelGroup *t_group);
  bool removeGroup(const id t_groupId);
  bool modifyGroup(const DmxChannelGroup *t_group);
  bool modifyGroup(const id t_groupId);

private :

  void addChannelGroup(id t_groupID,
                       QList<Ch_Id_Dmx> t_L_id_dmx);
  bool addChannel(const id t_groupID,
                  const Ch_Id_Dmx t_id_dmx);
  bool removeChannelGroup(id t_groupID);

signals :

  void channelLevelChangedFromGroup(id t_channelID,
                                    dmx t_level);

public slots :

  void groupLevelChanged(const id t_groupID,
                         const dmx t_level);

private :

  // id : group id , Ch_Id_Dmx : channelId _ stored level
  QMultiMap<id, Ch_Id_Dmx> m_MM_totalGroup;
  // channel Id , Gr_Id_Dmx : higher Group Id _ actual htp level
  QMap<id, Gr_Id_Dmx> m_M_channelLevel;
};

/******************************* ChannelEngine ***********************/

class ChannelEngine
    : public QObject
{

  Q_OBJECT

public :

  explicit ChannelEngine(QObject *parent = nullptr);

  ~ChannelEngine();



public slots :

  void onChannelLevelChangedFromGroup(id t_id,
                                      dmx t_level);

};

/****************************** Ch_Id_Dmx ********************************/

class Ch_Id_Dmx
{

public :

  explicit Ch_Id_Dmx(const id t_ID = NO_ID,
                     const dmx t_level = NULL_DMX)
    : m_ID(t_ID),
      m_level(t_level)
  {}

  virtual ~Ch_Id_Dmx(){}

  bool operator==(const Ch_Id_Dmx t_id_dmx) const
  { return ((t_id_dmx.getID() == m_ID)
            && (t_id_dmx.getLevel() == m_level)); }
  virtual bool isBrother(const Ch_Id_Dmx t_id_dmx) const
  { return (m_ID == t_id_dmx.getID()); }

  // WARNING : use isBrother() before these operators
  bool operator<(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level < t_id_dmx.getLevel()); }
  bool operator>(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level > t_id_dmx.getLevel()); }
  bool operator<=(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level <= t_id_dmx.getLevel()); }
  bool operator>=(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level >= t_id_dmx.getLevel()); }


  id getID() const{ return m_ID; }
  dmx getLevel() const{ return m_level; }

  void setID(const id t_ID){ m_ID = t_ID; }
  void setLevel(const dmx t_level){ m_level = t_level; }

  bool isValid() const{ return (m_ID > NO_ID); }

private :

  id m_ID = NO_ID;
  dmx m_level = NULL_DMX;

};

/****************************** Gr_Id_Dmx ********************************/

class Gr_Id_Dmx
    : public Ch_Id_Dmx
{

public :

  explicit Gr_Id_Dmx(const id t_ID = NO_ID,
                     const dmx t_level = NULL_DMX)
    : Ch_Id_Dmx(t_ID,
                t_level)
  {}

  ~Gr_Id_Dmx(){}

  bool isBrother(const Ch_Id_Dmx t_id_dmx) const override
  { return false; }
};

#endif // DMXENGINE_H

