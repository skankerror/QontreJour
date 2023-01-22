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

#include <QObject>
#include <QList>
#include "dmxchannel.h"

class DmxChannelGroup
    : public QObject
{

  Q_OBJECT

public:

  explicit DmxChannelGroup(QObject *parent = nullptr);

  // getters
  QList<std::pair<DmxChannel *, int> > getL_P_dmxChannel() const { return m_L_P_dmxChannel; }
  int getLevel() const { return m_level; }

  // setters
  void setLevel(int t_level);

  bool addDmxChannel(std::pair<DmxChannel *, int> t_P_dmxChannel);
  bool removeDmxChannel(DmxChannel *t_dmxChannel);
  bool setDmxChannelLevel(std::pair<DmxChannel *, int> t_P_dmxChannel);

  void clear();


signals:

  void v_p_dmxChannelChanged();
  void levelChanged(int);

public slots:

  void updateLevel(int t_level);

private:

  QList<std::pair<DmxChannel*, int>> m_L_P_dmxChannel; // un channel et son level
  int m_level;

  Q_PROPERTY(int getLevel
             READ getLevel
             WRITE setLevel
             NOTIFY levelChanged)
};

#endif // DMXCHANNELGROUP_H
