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

// NOTE : class for : dmxoutput, dmxchannel
//
// mother of :
// dmxchannelgroup, mother of :
// ** dmxscene

class DmxValue
    : public QObject
{

  Q_OBJECT

public:

  enum ValueType
  {
    Output,
    Channel,
    ChannelGroup,
    Scene,
    Unknown
  };
  Q_ENUM(ValueType)

  // enum for dmx channels
  enum SignalSenderType
  {
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

  explicit DmxValue(int t_ID,
                    QObject *parent = nullptr,
                    ValueType t_type = Unknown);

  explicit DmxValue(int t_ID,
                    QString &t_name,
                    QObject *parent = nullptr,
                    ValueType t_type = Unknown);

  explicit DmxValue(int t_universeID,
                    int t_ID,
                    QObject *parent = nullptr,
                    ValueType t_type = Unknown);

  explicit DmxValue(int t_universeID,
                    int t_ID,
                    QString &t_name,
                    QObject *parent = nullptr,
                    ValueType t_type = Unknown);


  virtual ~DmxValue();

// getters
  quint8 getLevel() const{  return m_level; }
  quint8 getMaxLevel() const { return m_maxLevel; }
  int getUniverseID() const { return m_universeID; }
  int getID() const { return m_ID; }
  QString getName() const{ return m_name; }
  QList<DmxValue *> getL_dmxValue() const{ return m_L_dmxValue; }
  DmxValue* getL_dmxValueAt(int t_index);
  ValueType getType() const{ return m_type; }
//  quint8 getStoredLevel() const{ return m_storedLevel; }
  quint8 getdirectChannelEditLevel() const{ return m_directChannelEditLevel; }
  quint8 getchannelGroupLevel() const{ return m_channelGroupLevel; }
  quint8 getselectedSceneLevel() const{ return m_selectedSceneLevel; }
  quint8 getnextSceneLevel() const{ return m_nextSceneLevel; }
  int getValuesCount() const { return m_L_dmxValue.size(); }

  // setters
  void setMaxLevel(quint8 t_maxLevel);
  void setUniverseID(int t_universeID) { m_universeID = t_universeID; }
  void setID(int t_ID);
  void setPropertyLevel(int t_level); // setter different from the slot, needed by property
  void setName(const QString &t_name) { m_name = t_name; }
  void setL_dmxValue(const QList<DmxValue *> &t_L_dmxValue){ m_L_dmxValue = t_L_dmxValue; }
  void setType(ValueType t_type){ m_type = t_type; }
//  void setStoredLevel(quint8 t_storedLevel){ m_storedLevel = t_storedLevel; }

  void addDmxValue(DmxValue *t_dmxValue);
  void addDmxValues(const QList<DmxValue *> t_L_dmxValue);
  bool removeDmxValue(const DmxValue *t_dmxValue);
  bool removeDmxValues(const QList<DmxValue *> t_L_dmxValue);
  void clearList();

private :

  void constructorMethod();
  void setChannelLevel(SignalSenderType t_senderType,
                       quint8 t_level);
  void setOutputLevel(SignalSenderType t_senderType,
                      quint8 t_level);

signals:

  void levelChanged(DmxValue::SignalSenderType,
                    quint8);  //m_level
//  void maxLevelChanged(quint8);// m_maxLevel
  void requestDmxUpdate(int, // m_outputID
                        quint8); // level
  void L_dmxValueChanged();

  void directChannelEditLevelChanged(quint8 directChannelEditLevel);
  void channelGroupLevelChanged(quint8 channelGroupLevel);
  void selectedSceneLevelChanged(quint8 selectedSceneLevel);
  void nextSceneLevelChanged(quint8 nextSceneLevel);

public slots:

  void setLevel(DmxValue::SignalSenderType t_senderType,
                quint8 t_level);
  void setDirectChannelEditLevel(quint8 t_directChannelEditLevel);
  void resetDirectChannelEditLevel(){ setDirectChannelEditLevel(0); }
  void setChannelGroupLevel(quint8 t_channelGroupLevel);
  void setSelectedSceneLevel(quint8 t_selectedSceneLevel);
  void setNextSceneLevel(quint8 t_nextSceneLevel);

protected :

  quint8 m_level;
  quint8 m_maxLevel = 255;
  quint8 m_directChannelEditLevel;
  quint8 m_channelGroupLevel;
  quint8 m_selectedSceneLevel;
  quint8 m_nextSceneLevel;
//  quint8 m_storedLevel;
  int m_ID;
  int m_universeID;
  QString m_name;
  QList<DmxValue *> m_L_dmxValue;
  ValueType m_type = Unknown;
  ChannelFlag m_flag = UnknownFlag;
  bool m_isDirectChannelEdited = false;

//private:

  Q_PROPERTY(quint8 level
             READ getLevel
             WRITE setPropertyLevel
             NOTIFY levelChanged)
};

#endif // DMXVALUE_H
