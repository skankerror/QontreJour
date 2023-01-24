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

#ifndef DMXOUTPUT_H
#define DMXOUTPUT_H

//#include <QObject>
//#include <QByteArray>
#include "dmxvalue.h"


class DmxOutput
    : public DmxValue
{

  Q_OBJECT

public:

  explicit DmxOutput(int t_universeID,
                     int t_ID,
                     QObject *parent = nullptr);

  ~DmxOutput()/* override*/;

  // getters
  quint8 getMaxLevel() const { return m_maxLevel; }
  int getUniverseID() const { return m_universeID; }

  // setters
  void setMaxLevel(quint8 t_maxLevel);
  void setUniverseID(int t_universeID) { m_universeID = t_universeID; }

signals:

  void maxLevelChanged(quint8);

private:

  quint8 m_maxLevel;
  int m_universeID;
  // TODO : add curves output

};

#endif // DMXOUTPUT_H
