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

signals:

private:

  QList<QPair<DmxChannel*, int>> v_p_dmxChannel;
  int m_level;

};

#endif // DMXCHANNELGROUP_H
