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

#include <QObject>
//#include <QByteArray>

class DmxOutput
    : public QObject
{

  Q_OBJECT

public:

  explicit DmxOutput(int t_universeID,
                     int t_outputID,
                     QObject *parent = nullptr);

  // getters
  int getLevel() const { return m_level; }
  int getMaxLevel() const { return m_maxLevel; }
  int getUniverseID() const { return m_universeID; }
  int getOutputID() const { return m_outputID; }

  // setters
  void setLevel(int t_level);
  void setMaxLevel(int t_maxLevel);
  void setUniverseID(int t_universeID) { m_universeID = t_universeID; }
  void setOutputID(int t_outputID) { m_outputID = t_outputID; }

signals:

  void levelChanged();
  void maxLevelChanged(int);

private:

  int m_level;
  int m_maxLevel;
  int m_universeID;
  int m_outputID;
  // TODO : add curves output

  Q_PROPERTY(int getLevel
             READ getLevel
             WRITE setLevel
             NOTIFY levelChanged)
};

#endif // DMXOUTPUT_H
