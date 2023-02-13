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

/******************************************************************************/

// class for :
// dmxoutput,
// dmxchannel,
// dmxchannelgroup
// ** mother of :
// ** dmxscene

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
    RootScene, // stored in manager
    Scene, // stored in root
    SubScene, // stored in scene
    UnknownValueType
  };
  Q_ENUM(ValueType)

  // enum for signals
  enum SignalSenderType
  {
    SubmasterSliderSender,
    ChannelGroupSender,
    DirectChannelEditSender,
    SelectedSceneSender,
    NextSceneSender,
    IndependantSender,
    ChannelToOutputSender,
    OutputToUniverseSender,
    UnknownSender
  };
  Q_ENUM(SignalSenderType)

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

  // cstr
  explicit DmxValue(ValueType t_type = UnknownValueType,
                    DmxValue *t_parent = nullptr);

  // dstr
  virtual ~DmxValue();

 /************************** GETTERS ***************************/
  // common
  id getID() const{ return m_ID; }
  uid getUniverseID() const { return m_universeID; }
  QString getName() const{ return m_name; }
  ValueType getType() const{ return m_type; }

  // all except root ones
  dmx getLevel() const{ return m_level; }

  // output
  dmx getMaxLevel() const { return m_maxLevel; }

  // channel
  dmx getdirectChannelEditLevel() const{ return m_directChannelEditLevel; }
  dmx getchannelGroupLevel() const{ return m_channelGroupLevel; }
  dmx getselectedSceneLevel() const{ return m_selectedSceneLevel; }
  dmx getnextSceneLevel() const{ return m_nextSceneLevel; }
  overdmx getOverOffset() const{ return m_overOffset; }
  ChannelFlag getChannelFlag() const{ return m_flag; }
  bool getIsDirectChannelEdited() const{ return m_isDirectChannelEdited; }

  // child values
  int getL_ChildValueSize() const{ return m_L_childValue.size(); }
  QList<DmxValue *> getL_childValue() const{ return m_L_childValue; }
  DmxValue *getChildValue(id t_id) const;
  // parent
  DmxValue *getParentValue() const{ return m_parentValue; }

  // controled values
  QList<DmxValue *> getL_controledValue() const{ return m_L_controledValue; }
  DmxValue * getControledValue(id t_index);
  dmx getControledValueLevel(id t_index);
  QList<dmx> getL_storedLevels() const{ return m_L_storedLevels; }
  int getL_ControledValueSize() const{ return m_L_controledValue.size(); }

  // for values controled by a widget
  QWidget *getAssignedWidget() const{ return m_assignedWidget; }


/****************************** SETTERS *********************************/

  // setters
  void setMaxLevel(dmx t_maxLevel);
  void setUniverseID(uid t_universeID) { m_universeID = t_universeID; }
  void setID(id t_ID);
  // setter different from the slot, needed by property
  void setPropertyLevel(dmx t_level);
  void setL_controledValue(const QList<DmxValue *> &t_m_L_controledValue)
  { m_L_controledValue = t_m_L_controledValue; }
  void setType(ValueType t_type){ m_type = t_type; }

  // meths for controled children
  void addControledChild(DmxValue *t_dmxValue);
  void addControledChildren(const QList<DmxValue *> t_m_L_controledValue);
  bool removeControledChild(const DmxValue *t_dmxValue);
  bool removeControledChildren(const QList<DmxValue *> t_m_L_controledValue);
  void clearControledList();

  void addChildValue(DmxValue *t_dmxValue);


protected :

  void setChannelLevel(SignalSenderType t_senderType,
                       dmx t_level/*,
                       id t_channelGroupID = -1*/);
  void setOutputLevel(SignalSenderType t_senderType,
                      dmx t_level);
  void setChannelGroupLevel(SignalSenderType t_senderType,
                            dmx t_level);

signals :

  void levelChanged(DmxValue::SignalSenderType,
                    dmx t_level);
  void requestDmxUpdate(uid t_universeID,
                        id t_outputID,
                        dmx t_level);
  void m_L_controledValueChanged();

  void directChannelEditLevelChanged(dmx directChannelEditLevel);
  void channelGroupLevelChanged(dmx channelGroupLevel);
  void selectedSceneLevelChanged(dmx selectedSceneLevel);
  void nextSceneLevelChanged(dmx nextSceneLevel);

  void blockDirectChannelSlider(dmx t_level);

public slots :

  void setLevel(DmxValue::SignalSenderType t_senderType,
                dmx t_level/*,
                id t_channelGroupId = -1*/);
  void setName(const QString &t_name) { m_name = t_name; }
  void setDirectChannelEditLevel(dmx t_directChannelEditLevel);
  void resetDirectChannelEditLevel(){ setDirectChannelEditLevel(0); }
  void setChannelGroupLevel(dmx t_channelGroupLevel);
  void setSelectedSceneLevel(dmx t_selectedSceneLevel);
  void setNextSceneLevel(dmx t_nextSceneLevel);
  void setControledChildLevel(DmxValue::SignalSenderType t_senderType,
                              int t_index,
                              dmx t_level);
  void setStoredLevel(int t_childIndex,
                      dmx t_level);

  void setFlag(DmxValue::ChannelFlag t_flag){ m_flag = t_flag; }

  void setParentValue(DmxValue *t_parentValue){ m_parentValue = t_parentValue; }

  void setL_childValue(const QList<DmxValue *> &t_L_childValue)
  { m_L_childValue = t_L_childValue;}
  void setL_storedLevels(const QList<dmx> &t_L_storedLevels)
  {  m_L_storedLevels = t_L_storedLevels; }
  void setIsDirectChannelEdited(bool t_isDirectChannelEdited)
  { m_isDirectChannelEdited = t_isDirectChannelEdited; }

  void setOverOffset(overdmx t_overOffset){ m_overOffset = t_overOffset; }

  void setAssignedWidget(QWidget *t_assignedWidget)
  { m_assignedWidget = t_assignedWidget; }

protected :

  // commons for all types :
  id m_ID = 0;
  uid m_universeID = -1;
  QString m_name = "Unknow Value";
  ValueType m_type = UnknownValueType;

  // for all except root ones
  dmx m_level = 0;

  // for output
  dmx m_maxLevel = 255;

  // for channel
  dmx m_directChannelEditLevel = 0;
  dmx m_channelGroupLevel = 0;
//  id m_highestGroupLevel = -1;
  dmx m_selectedSceneLevel = 0;
  dmx m_nextSceneLevel = 0;
  overdmx m_overOffset = 0;
  ChannelFlag m_flag = UnknownFlag;
  bool m_isDirectChannelEdited = false;


  // for all except subscene, output and channel group
  // children of instance
  // this will be output for root output
  // channels for root channel
  // channel(s) for channel group
  // channel group(s) and channel(s) for scene and subscene
  // scene for sub scene
  QList<DmxValue *> m_L_childValue;

  // for all except root Values, and output
  // value controled by instance
  // it's set by setLevel() slot that is emited and recieved
  // from value to value.
  // scene
  // subscene
  // setLevel for
  //   -channel group
  //  | and channel
  //  set level for
  //        channel
  // and then channel to output
  // all htp decisions are taken by channel
  // knowing if it's a direct edit
  // a channel group value
  // a channel group via scene
  // a scene, subscene, a nextscene
  // to determine wich value set to output and how present it
  // to mvc.
  // this is done with enum flags
  QList<DmxValue *> m_L_controledValue;

  // for controled channel or channel group levels
  // in scene, subscene or channelgroup
  QList<dmx> m_L_storedLevels;

  // parent value
  // for output and channels it will be root. universe will destroy
  // it on his destruction.
  // for scene this will be root scene
  // for subscene this will be a scene.
  // this is easy way to implement fu****g tree model in Qt.
  // so in a way a root scene is a sequence.
  DmxValue *m_parentValue = nullptr;

  // widget assigned to value, this may be a slider...
  QWidget *m_assignedWidget = nullptr;

  Q_PROPERTY(dmx level
             READ getLevel
             WRITE setPropertyLevel
             NOTIFY levelChanged)
};

#endif // DMXVALUE_H
