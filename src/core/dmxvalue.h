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


  explicit DmxValue(int t_ID,
                    QObject *parent = nullptr);

  explicit DmxValue(int t_ID,
                    QString &t_name,
                    QObject *parent = nullptr);

  explicit DmxValue(int t_universeID,
                    int t_ID,
                    QObject *parent = nullptr);

  explicit DmxValue(int t_universeID,
                    int t_ID,
                    QString &t_name,
                    QObject *parent = nullptr);


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
  quint8 getStoredLevel() const{ return m_storedLevel; }
  int getValuesCount() const { return m_L_dmxValue.size(); }

  // setters
  void setMaxLevel(quint8 t_maxLevel);
  void setUniverseID(int t_universeID) { m_universeID = t_universeID; }
  void setID(int t_ID);
  void setPropertyLevel(int t_level); // setter different from the slot, needed by property
  void setName(const QString &t_name) { m_name = t_name; }
  void setL_dmxValue(const QList<DmxValue *> &t_L_dmxValue){ m_L_dmxValue = t_L_dmxValue; }
  void setType(ValueType t_type){ m_type = t_type; }
  void setStoredLevel(quint8 t_storedLevel){ m_storedLevel = t_storedLevel; }


  void addDmxValue(DmxValue *t_dmxValue);
  void addDmxValues(const QList<DmxValue *> t_L_dmxValue);
  bool removeDmxValue(const DmxValue *t_dmxValue);
  bool removeDmxValues(const QList<DmxValue *> t_L_dmxValue);
  void clearList();



signals:

  void levelChanged(int,      //m_ID
                    quint8);  //m_level
  void maxLevelChanged(int,   // m_ID
                       quint8);// m_maxLevel
  void L_dmxValueChanged();

public slots:

  void setLevel(int t_ID,
                quint8 t_level);

protected :

  quint8 m_level;
  quint8 m_maxLevel = 255;
  quint8 m_storedLevel;
  int m_ID;
  int m_universeID;
  QString m_name;
  QList<DmxValue *> m_L_dmxValue;
  ValueType m_type = Unknown;

//private:

  Q_PROPERTY(quint8 level
             READ getLevel
             WRITE setPropertyLevel
             NOTIFY levelChanged)
};

#endif // DMXVALUE_H
