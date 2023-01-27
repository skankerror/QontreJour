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

#ifndef DMXSCENE_H
#define DMXSCENE_H

#include "dmxchannelgroup.h"


class DmxScene :
    public DmxChannelGroup
{

  Q_OBJECT

public:

  explicit DmxScene(int t_ID,
                    QString &t_name,
                    QObject *parent = nullptr,
                    DmxValue::ValueType t_type = Scene);

  ~DmxScene();

  // transform in DmxChannelGroup
  DmxChannelGroup *fromSceneToChannelGroup();

  // getters
  QString getNotes() const{ return m_notes; }
  double getTimeIn() const{ return m_timeIn; }
  double getTimeOut() const{ return m_timeOut; }
  double getDelayIn() const{ return m_delayIn; }
  double getDelayOut() const{ return m_delayOut; }
  QList<std::pair<DmxChannelGroup *, quint8>> getL_P_dmxChannelGroup() const{ return m_L_P_dmxChannelGroup; }

  // setters
  void setNotes(const QString &t_notes){ m_notes = t_notes; }
  void setTimeIn(double t_timeIn){ m_timeIn = t_timeIn; }
  void setTimeOut(double t_timeOut){ m_timeOut = t_timeOut; }
  void setDelayIn(double t_delayIn){ m_delayIn = t_delayIn; }
  void setDelayOut(double t_delayOut){ m_delayOut = t_delayOut; }

  //
  void addDmxChannelGroup(std::pair<DmxChannelGroup *, quint8> t_P_dmxChannelGroup);
  void removeDmxChannelGroup(DmxChannelGroup *t_dmxChannelGroup);
  void setDmxChannelGroupLevel(std::pair<DmxChannelGroup *, quint8> t_P_dmxChannelGroup);



public slots :

  void setL_P_dmxChannelGroup(const QList<std::pair<DmxChannelGroup *, quint8>> &t_L_P_dmxChannelGroup)
  { m_L_P_dmxChannelGroup = t_L_P_dmxChannelGroup; }

signals:

protected :

  QList<std::pair<DmxChannelGroup *, quint8>> m_L_P_dmxChannelGroup;
  QString m_notes;
  double m_timeIn;
  double m_timeOut;
  double m_delayIn;
  double m_delayOut;

};

#endif // DMXSCENE_H
