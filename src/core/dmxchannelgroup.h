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

#ifndef DMXCHANNELGROUP_H
#define DMXCHANNELGROUP_H

//#include <QObject>
#include <QList>
#include "dmxvalue.h"
//#include "dmxchannel.h"

class DmxChannelGroup
    : public DmxValue
{

  Q_OBJECT

public:

  explicit DmxChannelGroup(int t_ID,
                           QObject *parent = nullptr);

  explicit DmxChannelGroup(int t_ID,
                           QString &t_name,
                           QObject *parent = nullptr);


  virtual ~DmxChannelGroup();

  // getters
  QList<std::pair<DmxValue *, quint8> > getL_P_dmxChannel() const { return m_L_P_dmxChannel; }

  // public fonctions
  bool addDmxChannel(std::pair<DmxValue *, quint8> t_P_dmxChannel);
  bool removeDmxChannel(DmxValue *t_dmxChannel);
  bool setDmxChannelLevel(std::pair<DmxValue *, quint8> t_P_dmxChannel);

  void clear();


signals:

  void L_P_dmxChannelChanged();

public slots:

  void updateLevel(int t_level);

protected :

  QList<std::pair<DmxValue *, quint8>> m_L_P_dmxChannel; // un channel et son level

};

#endif // DMXCHANNELGROUP_H
