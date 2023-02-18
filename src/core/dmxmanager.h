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

#ifndef DMXMANAGER_H
#define DMXMANAGER_H

#include <QObject>
#include <QString>
#include "qdmxlib/QDmxManager"
#include "dmxvalue.h"
//#include "dmxscene.h"
#include "dmxuniverse.h"

#define MANAGER DmxManager::instance()
#define GET_CHANNEL_GROUP(x) MANAGER->getChannelGroup(x)
#define GET_CHANNEL(x) MANAGER->getChannel(x)
#define GET_OUTPUT(x,y) MANAGER->getOutput(Uid_Id(x,y))
#define GET_UNIVERSE_COUNT MANAGER->getUniverseCount()
#define NULL_UID_ID Uid_Id(NO_UID,NO_ID)

class DmxPatch;
class Uid_Id;

class DmxManager
    : public QObject
{

  Q_OBJECT

public :

  enum HwPortType
  {
    HwInput,
    HwOutput
  }; Q_ENUM(HwPortType)

  enum WidgetType
  {
    DmxSlider,
    DmxTableView,
    UnknownWidgetType
  }; Q_ENUM(WidgetType)


  static DmxManager *instance();

  ~DmxManager();

  // getters
  QStringList getAvailableDriversNames() const;
  QStringList getAvailableDevicesNames(const QString &t_driverString);
  int getUniverseCount() const{ return m_L_universe.size() ;}
  DmxOutput *getOutput(Uid_Id t_output_Uid_Id);
  RootValue *getRootChannel() const{ return m_rootChannel; }
  DmxChannel *getChannel(id t_channelId);
  int getChannelCount() const{ return m_rootChannel->getL_childValueSize(); }
  RootValue *getRootChannelGroup() const{ return m_rootChannelGroup; }
  DmxChannelGroup *getChannelGroup(id t_groupId);
  int getChannelGroupCount() const{ return m_rootChannelGroup->getL_childValueSize(); }

  // create everything we need
  bool createUniverse(uid t_universeID);
  DmxChannelGroup *createChannelGroup(QList<DmxChannel *> t_L_channel);

  // patch interface
  void setStraightPatch(const uid t_uid); // one universe
  void setStraightPatch(const QList<uid> t_L_uid); // several universes
  void setStraightPatch(); // all universes
  void clearPatch();
  void patchOutputToChannel(DmxChannel *t_channel,
                            DmxOutput *t_output);
  void patchOutputListToChannel(DmxChannel *t_channel,
                                QList<DmxOutput *> t_L_output);
  void unpatchOutputFromChannel(DmxChannel *t_channel,
                                DmxOutput *t_output);
  void unpatchOutputListFromChannel(DmxChannel *t_channel,
                                    QList<DmxOutput *> t_L_output);
  void unpatchOutput(DmxOutput *t_output);
  void unpatchOutputList(QList<DmxOutput *> t_L_output);
  void clearChannelPatch(DmxChannel *t_channel);
  void clearChannelListPatch(QList<DmxChannel *> t_L_channel);

  // hardware connection
  bool hwConnect(DmxManager::HwPortType t_type,
                 QString &t_driver,
                 QString &t_device,
                 quint8 t_port,
                 uid t_ID);
  bool hwDisconnect(uid t_ID);


private :

  explicit DmxManager(QObject *parent = nullptr);
  QList<QDmxDriver *> getAvailableDrivers() const;
  QList<QDmxDevice *> getAvailableDevices(const QString &t_driverString);
  RootValue *getRootOutput(const uid t_uid) const;
  QList<RootValue *> getL_rootOutput() const;

signals :

public slots :

private slots :

private :

  QDmxManager *m_hwManager;
  DmxPatch *m_dmxPatch;
  QList<DmxUniverse *> m_L_universe;
  RootValue *m_rootChannel;
  RootValue *m_rootChannelGroup;

};

/******************************** Uid_Id *************************************/

class Uid_Id
{

public :

  explicit Uid_Id(const uid t_uid = NO_UID,
                  const id t_id = NO_ID)
    : m_universeID(t_uid),
      m_outputID(t_id)
  {}

  explicit Uid_Id(const DmxOutput *t_output);

  explicit Uid_Id(const QString &t_string);

  ~Uid_Id(){}

  bool operator==(const Uid_Id t_Uid_Id) const
  {
    return ((t_Uid_Id.getUniverseID() == m_universeID)
            && (t_Uid_Id.getOutputID() == m_outputID));
  }

  uid getUniverseID() const{ return m_universeID; }
  id getOutputID() const{ return m_outputID; }

  void setUniverseID(const uid t_universeID){ m_universeID = t_universeID; }
  void setOutputID(const id t_outputID){ m_outputID = t_outputID; }

  QString toString() const
  {
    return QString("%1.%2")
        .arg(m_universeID)
        .arg(m_outputID);
  }

  static QString UidtoString(const Uid_Id t_uid_id)
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

  void setMM_patch(const QMultiMap<id, Uid_Id> &t_MM_patch)
  { m_MM_patch = t_MM_patch; }

  void clearPatch();
  bool clearChannel(const id t_channelID);
  bool addOutputToChannel(const id t_channelID,
                          const Uid_Id t_outputUid_Id);
  void addOutputListToChannel(const id t_channelId,
                              const QList<Uid_Id> t_L_outputUid_Id);
  bool removeOutput(const Uid_Id t_outputUid_Id);
  void removeOutputList(const QList<Uid_Id> t_L_outputUid_Id);
  bool removeOutputFromChannel(const id t_channelID,
                               const Uid_Id t_outputUid_Id);
  void removeOutputListFromChannel(const id t_channelID,
                                   const QList<Uid_Id> t_L_outputUid_Id);

private :

  QMultiMap<id, Uid_Id> m_MM_patch;

};

#endif // DMXMANAGER_H
