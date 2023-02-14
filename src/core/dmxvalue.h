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

#ifndef DMXVALUE_H
#define DMXVALUE_H

#include <QObject>
#include <QString>
#include <QWidget>

// 0 to 255, value for dmx levels
typedef quint8 dmx ;

// needed when editing channel group level
// when one channel is at 0 or 255, the editing group can still change
// when need to keep decay between unchanged values, and those who are still changing
typedef qint16 overdmx;

// id for every output, channel, channel group, etc...
// not unsigned cos it can be -1 to specify no id
typedef qint16 id;

// id for universe. signed cos it can be -1 for channel group, scene, etc...
typedef qint16 uid;

#define NO_ID -1
#define NO_UID -1

#define DEFAULT_OUTPUT_NAME "OUT"
#define DEFAULT_CHANNEL_NAME "CH"
#define DEFAULT_GROUP_NAME "GROUP"

/******************************** DMXVALUE **************************************/

class DmxValue
    : public QObject
{

  Q_OBJECT

public :

  enum ValueType
  {
    RootOutput, // stored in universe
    Output, // stored in root
    RootChannel, // stored in universe
    Channel, // stored in root
    RootChannelGroup, // stored in manager
    ChannelGroup, // stored in root
    UnknownValueType
  };
  Q_ENUM(ValueType)



  explicit DmxValue(ValueType t_type = UnknownValueType,
                    DmxValue *t_parent = nullptr);

  virtual ~DmxValue();

  id getID() const{ return m_ID; }
  uid getUniverseID() const { return m_universeID; }
  QString getName() const{ return m_name; }
  ValueType getType() const{ return m_type; }

  void setID(id t_ID){ m_ID = t_ID; }
  void setUniverseID(uid t_universeID) { m_universeID = t_universeID; }
  void setName(const QString &t_name) { m_name = t_name; }
  void setType(ValueType t_type){ m_type = t_type; }

protected :

  id m_ID = NO_ID;
  uid m_universeID = NO_UID;
  QString m_name = "Unknow Value";
  ValueType m_type = UnknownValueType;

};

/********************************* ROOTVALUE *************************************/

class LeveledValue;

class RootValue
    : public DmxValue
{

  Q_OBJECT

public :

  RootValue(ValueType t_type = UnknownValueType,
            DmxValue *t_parent = nullptr);

  ~RootValue();

  // child values
  int getL_ChildValueSize() const{ return m_L_childValue.size(); }
  QList<LeveledValue *> getL_childValue() const{ return m_L_childValue; }
  LeveledValue *getChildValue(id t_id) const;

  void setL_childValue(const QList<LeveledValue *> &t_L_childValue)
  { m_L_childValue = t_L_childValue; }

  void addChildValue(LeveledValue *t_value);
  void addChlidList(QList<LeveledValue *> t_L_value);
  void removeChildValue(id t_index);
  void removeChildValueList(QList<id> t_L_index);

private :

  QList<LeveledValue *> m_L_childValue;

};

/******************************** LEVELEDVALUE **************************************/

class LeveledValue
    : public DmxValue
{

  Q_OBJECT

public :

  LeveledValue(ValueType t_type = UnknownValueType,
               RootValue *t_parent = nullptr);

  virtual ~LeveledValue();

  dmx getLevel() const{ return m_level; }
  RootValue *getParentValue() const{ return m_parentValue; }
  QWidget *getAssignedWidget() const{ return m_assignedWidget; }

signals :

  void levelChanged(dmx level);

public slots :

  void setLevel(dmx t_level);
  // TODO: Adapt
  void resetLevel(){ setLevel({}); }
  void setParentValue(RootValue *t_parentValue)
  { m_parentValue = t_parentValue; }
  void setAssignedWidget(QWidget *t_assignedWidget)
  { m_assignedWidget = t_assignedWidget; }

protected :

  dmx m_level = 0;
  RootValue *m_parentValue = nullptr;

  // widget assigned to value, this may be a slider...
  // rotate button...
  QWidget *m_assignedWidget = nullptr;

  Q_PROPERTY(dmx level
             READ getLevel
             WRITE setLevel
             RESET resetLevel
             NOTIFY levelChanged)
};

/********************************* DMXOUTPUT *************************************/

class DmxOutput
    : public LeveledValue
{

  Q_OBJECT

public :

  DmxOutput(ValueType t_type = Output,
            RootValue *t_parent = nullptr);

  virtual ~DmxOutput();

  dmx getMaxLevel() const{ return m_maxLevel; }
  bool getIsParked() const{ return m_isParked; }

signals :

  void outputRequestUpdate(id t_id,
                           dmx t_level);

public slots :

  void setMaxLevel(dmx t_maxLevel){ m_maxLevel = t_maxLevel; }
  void setIsParked(bool t_isParked){ m_isParked = t_isParked; }

private :

  dmx m_maxLevel = 255;
  bool m_isParked;

};

/********************************** DMXCHANNEL ************************************/

class DmxChannel
    : public LeveledValue
{

  Q_OBJECT

public :

  enum ChannelFlag
  {
    SelectedSceneFlag,
    DirectChannelFlag,
    ChannelGroupFlag,
    ParkedFlag,
    IndependantFlag,
    UnknownFlag
  };
  Q_ENUM(ChannelFlag)

  DmxChannel(ValueType t_type = Channel,
             RootValue *t_parent = nullptr);

  virtual ~DmxChannel();

  dmx getdirectChannelEditLevel() const{ return m_directChannelEditLevel; }
  dmx getchannelGroupLevel() const{ return m_channelGroupLevel; }
  dmx getselectedSceneLevel() const{ return m_selectedSceneLevel; }
  dmx getnextSceneLevel() const{ return m_nextSceneLevel; }
  overdmx getOverOffset() const{ return m_overOffset; }
  ChannelFlag getChannelFlag() const{ return m_flag; }
  bool getIsDirectChannelEdited() const{ return m_isDirectChannelEdited; }

  // controled values
  QList<DmxOutput *> getL_controledOutput() const{ return m_L_controledOutput; }
  DmxOutput *getControledOutput(const id t_index);
  dmx getControledOutputLevel(const id t_index);
  int getL_ControledOutputSize() const{ return m_L_controledOutput.size(); }

  // setters
  void setL_controledOutput(QList<DmxOutput *> &t_L_controledOutput)
  { m_L_controledOutput = t_L_controledOutput; }
  void setChannelFlag(DmxChannel::ChannelFlag t_flag){ m_flag = t_flag; }
  void setIsDirectChannelEdited(bool t_isDirectChannelEdited)
  { m_isDirectChannelEdited = t_isDirectChannelEdited; }
  void setOverOffset(overdmx t_overOffset){ m_overOffset = t_overOffset; }


  void addOutput(DmxOutput *t_dmxOutput);
  void addOutputList(QList<DmxOutput *> t_L_controledOutput);
  void removeOutput(const id t_index);
  void removeOutputList(const QList<id> t_L_index);
  void clearControledOutput();


private :

  QList<DmxOutput *>m_L_controledOutput;

  dmx m_directChannelEditLevel = 0;
  dmx m_channelGroupLevel = 0;
  dmx m_selectedSceneLevel = 0;
  dmx m_nextSceneLevel = 0;
  overdmx m_overOffset = 0;
  ChannelFlag m_flag = UnknownFlag;
  bool m_isDirectChannelEdited = false;

};

/********************************** DMXCHANNELGROUP ************************************/

class DmxChannelGroup
    : public LeveledValue

{

  Q_OBJECT

public :

  DmxChannelGroup(ValueType t_type = ChannelGroup,
                  RootValue *t_parent = nullptr);

  ~DmxChannelGroup();

  // controled values
  QList<DmxChannel *> getL_controledChannel() const{ return m_L_controledChannel; }
  DmxChannel *getControledChannel(const id t_index);
  dmx getControledChannelLevel(const id t_index);
  dmx getControledChannelStoredLevel(const id t_index);
  int getL_ControledChannelSize() const{ return m_L_controledChannel.size(); }

  // setters
  void setL_controledChannel(QList<DmxChannel *> &t_L_controledChannel)
  { m_L_controledChannel = t_L_controledChannel; }
  void setL_storedLevel(QList<dmx> t_L_storedLevel)
  { m_L_storedLevel = t_L_storedLevel; }

  void addChannel(DmxChannel *t_dmxChannel,
                  dmx t_storedLevel);
  void addChannelList(QList<DmxChannel *> t_L_controledChannel,
                      QList<dmx> t_L_storedLevel);
  void removeChannel(const id t_index);
  void removeChannelList(const QList<id> t_L_index);
  void clearControledChannel();


private :

  QList<DmxChannel *> m_L_controledChannel;
  QList<dmx> m_L_storedLevel;

};

//  // enum for signals
//  enum SignalSenderType
//  {
//    SubmasterSliderSender,
//    ChannelGroupSender,
//    DirectChannelEditSender,
//    SelectedSceneSender,
//    NextSceneSender,
//    IndependantSender,
//    ChannelToOutputSender,
//    OutputToUniverseSender,
//    UnknownSender
//  };
//  Q_ENUM(SignalSenderType)

#endif // DMXVALUE_H
