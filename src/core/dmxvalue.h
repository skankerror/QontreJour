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
#include "../qontrejour.h"

/******************************** DMXVALUE **************************************/

class DmxValue
    : public QObject
{

  Q_OBJECT

public :

  Q_ENUM(ValueType)

  explicit DmxValue(ValueType t_type = UnknownValueType,
                    DmxValue *t_parent = nullptr)
      : QObject(t_parent),
      m_type(t_type)
  {}

  virtual ~DmxValue(){}

  QString getName() const{ return m_name; }
  ValueType getType() const{ return m_type; }

  void setName(const QString &t_name) { m_name = t_name; }
  void setType(const ValueType t_type){ m_type = t_type; }

protected :

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
  int getL_childValueSize() const{ return m_L_childValue.size(); }
  QList<LeveledValue *> getL_childValue() const{ return m_L_childValue; }
  LeveledValue *getChildValue(id t_id) const;

  void setL_childValue(const QList<LeveledValue *> &t_L_childValue)
  { m_L_childValue = t_L_childValue; }

  void addChildValue(LeveledValue *t_value);
  void addChlidList(const QList<LeveledValue *> t_L_value);
  void removeChildValue(const id t_index);
  void removeChildValueList(const QList<id> t_L_index);

private :

  QList<LeveledValue *> m_L_childValue;

};

/********************************* ROOTOUTPUT *************************************/

class RootOutput
    : public RootValue,
      public UniversedValue
{

public :

  RootOutput(ValueType t_type = ValueType::RootOutputType,
             DmxValue *t_parent = nullptr)
      : RootValue(t_type,
                  t_parent),
      UniversedValue()
  {}
};

/******************************** LEVELEDVALUE **************************************/

class LeveledValue
    : public DmxValue,
      public IdedValue
{

  Q_OBJECT

public :

  LeveledValue(ValueType t_type = UnknownValueType,
               RootValue *t_parent = nullptr)  : DmxValue(t_type,
                 t_parent),
      IdedValue()
  {}

  virtual ~LeveledValue(){}

  dmx getLevel() const{ return m_level; }
  RootValue *getParentValue() const{ return m_parentValue; }
  QWidget *getAssignedWidget() const{ return m_assignedWidget; }

signals :

  void levelChanged(id t_id,
                    dmx t_level);

public slots :

  virtual void setLevel(dmx t_level)
  { m_level = t_level;
    emit levelChanged(m_id,
                      m_level); }

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

};

/********************************* DMXOUTPUT *************************************/

class DmxChannel;

class DmxOutput
    : public LeveledValue,
      public UniversedValue
{

  Q_OBJECT

public :

  DmxOutput(ValueType t_type = OutputType,
            RootOutput *t_parent = nullptr)
      : LeveledValue(t_type,
                     t_parent)
  {
    setName(DEFAULT_OUTPUT_NAME);
  }
  virtual ~DmxOutput(){}

  dmx getMaxLevel() const{ return m_maxLevel; }
  bool getIsParked() const{ return m_isParked; }
  DmxChannel *getChannelControler() const{ return m_channelControler; }
  Uid_Id getUid_Id()const{ return Uid_Id(getuid(), getid()); }

  void setChannelControler(DmxChannel *t_channelControler)
  { m_channelControler = t_channelControler; }

signals :

  void outputRequestUpdate(uid t_uid,
                           id t_id,
                           dmx t_level);

public slots :

  void setLevel(dmx t_level) override;
  void setMaxLevel(dmx t_maxLevel){ m_maxLevel = t_maxLevel; }
  void setIsParked(bool t_isParked){ m_isParked = t_isParked; }

private :

  dmx m_maxLevel = MAX_DMX;
  bool m_isParked;
  DmxChannel *m_channelControler = nullptr;

};

/********************************** DMXCHANNEL ************************************/

class DmxChannel
    : public LeveledValue
{

  Q_OBJECT

public :

  DmxChannel(ValueType t_type = ChannelType,
             RootValue *t_parent = nullptr)
      : LeveledValue(t_type,
                     t_parent)
  { setName(DEFAULT_CHANNEL_NAME); }


  ~DmxChannel(){ clearControledOutput(); }

  // controled values
  QList<DmxOutput *> getL_controledOutput() const{ return m_L_controledOutput; }
  DmxOutput *getControledOutput(const id t_index);
  dmx getControledOutputLevel(const id t_index);
  int getL_controledOutputSize() const{ return m_L_controledOutput.size(); }
  ChannelDataFlag getChannelDataFlag() const
  { return m_channelDataFlag; }

  // setters
  void setL_controledOutput(const QList<DmxOutput *> &t_L_controledOutput)
  { m_L_controledOutput = t_L_controledOutput; }
  void setChannelDataFlag(ChannelDataFlag t_channelDataFlag)
  { m_channelDataFlag = t_channelDataFlag; }

  void addOutput(DmxOutput *t_dmxOutput);
  void addOutput(Uid_Id t_Uid_Id);
  void addOutputList(const QList<DmxOutput *> t_L_controledOutput);
  void removeOutput(const DmxOutput *t_output);
  void removeOutput(const Uid_Id t_Uid_Id);
  void removeOutputList(const QList<DmxOutput *> t_L_output);
  void removeOutputList(const QList<Uid_Id> t_L_Uid_Id);
  void clearControledOutput();


private :

  QList<DmxOutput *>m_L_controledOutput;
  ChannelDataFlag m_channelDataFlag = ChannelDataFlag::UnknownFlag;

};
Q_DECLARE_METATYPE(DmxChannel)

/********************************** DMXCHANNELGROUP ************************************/

class DmxChannelGroup
    : public LeveledValue
{

  Q_OBJECT

public :

  DmxChannelGroup(ValueType t_type = ChannelGroup,
                  RootValue *t_parent = nullptr);

  ~DmxChannelGroup();

  dmx getControledChannelStoredLevel(const id t_id);
  dmx getControledChannelStoredLevel(DmxChannel *m_channel);
  int getL_controledChannelSize() const
  { return m_H_controledChannel_storedLevel.size(); }
  QHash<DmxChannel *, dmx> getH_controledChannel_storedLevel() const
  { return m_H_controledChannel_storedLevel; }

  // setters
  void setH_controledChannel_storedLevel(const QHash<DmxChannel *, dmx> &t_H_controledChannel_storedLevel)
  { m_H_controledChannel_storedLevel = t_H_controledChannel_storedLevel; }

  void addChannel(DmxChannel *t_dmxChannel,
                  const dmx t_storedLevel);
  void addChannel(const id t_id,
                  const dmx t_storedLevel);
  // TODO :
  //  void addChannelList(const QMap<id ,dmx> t_M_controledChannelId_storedLevel);
  //  void addChannelList(const QHash<DmxChannel *,dmx> t_M_controledChannel_storedLevel);
  void removeChannel(DmxChannel * t_channel);
  void removeChannel(const id t_id);
  void removeChannelList(const QList<DmxChannel *> t_L_channel);
  void removeChannelList(const QList<id> t_L_id);
  void clearControledChannel();

private :

  QHash<DmxChannel *,dmx> m_H_controledChannel_storedLevel;

};

/****************************** DmxPatch ******************************/

class DmxPatch
{

public :

  explicit DmxPatch(){}

  ~DmxPatch(){}

  QMultiMap<id, Uid_Id> getMM_patch() const{ return m_MM_patch; }
  QList<Uid_Id> getL_Uid_Id(id t_channelID)
  { return m_MM_patch.values(t_channelID); }

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

#endif // DMXVALUE_H
