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

// NOTE : class for :
// dmxoutput,
// dmxchannel,
// dmxchannelgroup
// ** mother of :
// ** dmxscene

class DmxValue
    : public QObject
{

  Q_OBJECT

public:

  enum ValueType
  {
    RootOutput,
    Output,
    RootChannel,
    Channel,
    ChannelGroup,
    RootScene,
    Scene,
    SubScene,
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

  // getters
  quint8 getLevel() const{  return m_level; }
  quint8 getMaxLevel() const { return m_maxLevel; }
  int getUniverseID() const { return m_universeID; }
  int getID() const{ return m_ID; }
  QString getName() const{ return m_name; }
  QList<DmxValue *> getL_controledValue() const{ return m_L_controledValue; }
  DmxValue * getControledChild(int t_index);
  ValueType getType() const{ return m_type; }
  quint8 getdirectChannelEditLevel() const{ return m_directChannelEditLevel; }
  quint8 getchannelGroupLevel() const{ return m_channelGroupLevel; }
  quint8 getselectedSceneLevel() const{ return m_selectedSceneLevel; }
  quint8 getnextSceneLevel() const{ return m_nextSceneLevel; }
  ChannelFlag getChannelFlag() const{ return m_flag; }
  int getControledValueSize() const{ return m_L_controledValue.size(); }
  int getChildValueSize() const{ return m_L_childValue.size(); }
  quint8 getControledChildLevel(int t_index);
  DmxValue *getParentValue() const{ return m_parentValue;}
  QList<DmxValue *> getL_childValue() const{ return m_L_childValue;}
  QList<quint8> getL_storedLevels() const{ return m_L_storedLevels; }
  ChannelFlag getFlag() const{ return m_flag; }
  bool getIsDirectChannelEdited() const{ return m_isDirectChannelEdited; }

  // setters
  void setMaxLevel(quint8 t_maxLevel);
  void setUniverseID(int t_universeID) { m_universeID = t_universeID; }
  void setID(int t_ID);
  // setter different from the slot, needed by property
  void setPropertyLevel(int t_level);
  void setName(const QString &t_name) { m_name = t_name; }
  void setL_controledValue(const QList<DmxValue *> &t_m_L_controledValue)
  { m_L_controledValue = t_m_L_controledValue; }
  void setType(ValueType t_type){ m_type = t_type; }

  // meths for controled children
  void addControledChild(DmxValue *t_dmxValue);
  void addControledChildren(const QList<DmxValue *> t_m_L_controledValue);
  bool removeControledChild(const DmxValue *t_dmxValue);
  bool removeControledChildren(const QList<DmxValue *> t_m_L_controledValue);
  void clearList();

  void addChildValue(DmxValue *t_dmxValue);

protected :

  void setChannelLevel(SignalSenderType t_senderType,
                       quint8 t_level);
  void setOutputLevel(SignalSenderType t_senderType,
                      quint8 t_level);
  void setChannelGroupLevel(SignalSenderType t_senderType,
                            quint8 t_level);

signals:

  void levelChanged(DmxValue::SignalSenderType,
                    quint8 t_level);
  void requestDmxUpdate(int m_outputID,
                        quint8 t_level);
  void m_L_controledValueChanged();

  void directChannelEditLevelChanged(quint8 directChannelEditLevel);
  void channelGroupLevelChanged(quint8 channelGroupLevel);
  void selectedSceneLevelChanged(quint8 selectedSceneLevel);
  void nextSceneLevelChanged(quint8 nextSceneLevel);

  void blockDirectChannelSlider(quint8 t_level);

public slots:

  /*virtual */void setLevel(DmxValue::SignalSenderType t_senderType,
                        quint8 t_level);
  void setDirectChannelEditLevel(quint8 t_directChannelEditLevel);
  void resetDirectChannelEditLevel(){ setDirectChannelEditLevel(0); }
  void setChannelGroupLevel(quint8 t_channelGroupLevel);
  void setSelectedSceneLevel(quint8 t_selectedSceneLevel);
  void setNextSceneLevel(quint8 t_nextSceneLevel);
  void setControledChildLevel(DmxValue::SignalSenderType t_senderType,
                              int t_index,
                              quint8 t_level);
  void setStoredLevel(int t_childIndex,
                      quint8 t_level);

  void setFlag(DmxValue::ChannelFlag t_flag){ m_flag = t_flag; }

  void setParentValue(DmxValue *t_parentValue){ m_parentValue = t_parentValue; }

  void setL_childValue(const QList<DmxValue *> &t_L_childValue)
  { m_L_childValue = t_L_childValue;}
  void setL_storedLevels(const QList<quint8> &t_L_storedLevels)
  {  m_L_storedLevels = t_L_storedLevels; }
  void setIsDirectChannelEdited(bool t_isDirectChannelEdited)
  { m_isDirectChannelEdited = t_isDirectChannelEdited; }

protected :

  // commons for all types :
  int m_ID;
  int m_universeID;
  QString m_name;
  ValueType m_type = UnknownValueType;

  // for all except root ones
  quint8 m_level = 0;

  // for output
  quint8 m_maxLevel = 255;

  // for channel
  quint8 m_directChannelEditLevel;
  quint8 m_channelGroupLevel;
  quint8 m_selectedSceneLevel;
  quint8 m_nextSceneLevel;
  ChannelFlag m_flag = UnknownFlag;
  bool m_isDirectChannelEdited = false;

  // faire aussi une autre value pour channel group ?
  // si ça reçoit de plusieurs sources ?

  // for all except subscene output channel group
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
  QList<quint8> m_L_storedLevels;

  // parent value
  // for output and channels it will be qobj child of universe which will destroy
  // it on his destruction.
  // for scene this will be root scene
  // for subscene this will be a scene.
  // this is easy way to implement fu****g tree model in Qt.
  // so in a way a root scene is a sequence.
  DmxValue *m_parentValue;

  Q_PROPERTY(quint8 level
             READ getLevel
             WRITE setPropertyLevel
             NOTIFY levelChanged)
};

#endif // DMXVALUE_H
