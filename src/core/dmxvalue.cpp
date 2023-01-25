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

#include "dmxvalue.h"

DmxValue::DmxValue(int t_ID,
                   QObject *parent)
  : QObject(parent)
{}

DmxValue::~DmxValue()
{}

void DmxValue::setLevel(int t_ID,
                        quint8 t_level)
{
  Q_UNUSED(t_ID)
  if (m_level == t_level
      || (t_level < 0)
      || (t_level > 255))
    return;

  m_level = t_level;

  emit levelChanged(m_ID,
                    m_level);
}

void DmxValue::setID(int t_ID)
{
  if (m_ID == t_ID)
    return;
  m_ID = t_ID;
}

void DmxValue::setPropertyLevel(int t_level)
{
  if (m_level == t_level
      || (t_level < 0)
      || (t_level > 255))
    return;

  m_level = t_level;

  emit levelChanged(m_ID,
                    m_level);

}
