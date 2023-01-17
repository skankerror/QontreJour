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

#include "dmxoutput.h"

DmxOutput::DmxOutput(int t_universeID,
                     int t_outputID,
                     QObject *parent)
  : QObject(parent),
    m_level(0),
    m_maxLevel(255),
    m_universeID(t_universeID),
    m_outputID(t_outputID)
{}

void DmxOutput::setLevel(int t_level)
{
  if (m_level == t_level || t_level > m_maxLevel)
    return;
  m_level = t_level;
  emit levelChanged();
}

void DmxOutput::setMaxLevel(int t_maxLevel)
{
  if (m_maxLevel == t_maxLevel)
    return;
  m_maxLevel = t_maxLevel;
  emit maxLevelChanged(m_maxLevel);
}
