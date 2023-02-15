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
#include <QString>
#include "dmxvalue.h"

#define ENGINE DmxEngine::instance()

class DmxPatch;

class DmxEngine
    : public QObject
{

  Q_OBJECT

public :

  static DmxEngine *instance();

  ~DmxEngine();

  // for one universe
  void straightPatch(RootValue *t_rootChannel,
                     RootValue * t_rootOutput);
  // for several universes
  void straightPatch(RootValue *t_rootChannel,
                     QList<RootValue *> t_L_rootOutput);

  void clearPatch(RootValue *t_rootChannel);

private :

  explicit DmxEngine(QObject *parent = nullptr);

private :

  DmxPatch *m_dmxPatch;

};

/******************************** Uid_Id *************************************/

class Uid_Id
{

public :

  explicit Uid_Id(uid t_uid = NO_UID,
                  id t_id = NO_ID)
    : m_universeID(t_uid),
      m_outputID(t_id)
  {}

  explicit Uid_Id(QString &t_string);

  ~Uid_Id(){}

  uid getUniverseID() const{ return m_universeID; }
  id getOutputID() const{ return m_outputID; }

  void setUniverseID(uid t_universeID){ m_universeID = t_universeID; }
  void setOutputID(id t_outputID){ m_outputID = t_outputID; }

  QString toString()
  {
    return QString("%1.%2")
        .arg(m_universeID)
        .arg(m_outputID);
  }

  static QString UidtoString(Uid_Id t_uid_id)
  {
    return QString("%1.%2")
        .arg(t_uid_id.getUniverseID())
        .arg(t_uid_id.getOutputID());
  }

private :

  uid m_universeID = NO_UID;
  id m_outputID = NO_ID;

};

 /****************************** DmxPatch ******************************/

class DmxPatch
{

public :

  explicit DmxPatch(){}

  ~DmxPatch(){}

  QMultiMap<id, Uid_Id> getMM_patch() const{ return m_MM_patch; }

  void setMM_patch(const QMultiMap<id, Uid_Id> &t_MM_patch){ m_MM_patch = t_MM_patch; }

  void clearPatch();
  void clearChannel(id t_channelID);
  void addOutputToChannel(id t_channelID,
                          Uid_Id t_outputUid_Id);
  void addOutputListToChannel(id t_channelId,
                              QList<Uid_Id> t_L_outputUid_Id);
  void removeOutputFromChannel(id t_channelID,
                               Uid_Id t_outputUid_Id);
  void removeOutputListFromChannel(id t_channelID,
                                   QList<Uid_Id> t_L_outputUid_Id);

private :

  QMultiMap<id, Uid_Id> m_MM_patch;

};

#endif // DMXENGINE_H
