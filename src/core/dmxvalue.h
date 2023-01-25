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

class DmxValue : public QObject
{

  Q_OBJECT

public:

  explicit DmxValue(int t_ID,
                    QObject *parent = nullptr);

  virtual ~DmxValue()/* = 0*/;

// getters
  quint8 getLevel() const{  return m_level; }
  int getID() const { return m_ID; }

  // setters
  void setID(int t_ID);
  void setPropertyLevel(int t_level); // setter different from the slot, needed by property

signals:

  void levelChanged(int,      //m_ID
                    quint8);  //m_level

public slots:

  void setLevel(int t_ID,
                quint8 t_level);

protected :

  quint8 m_level;
  int m_ID;

//private:

  Q_PROPERTY(quint8 level
             READ getLevel
             WRITE setPropertyLevel
             NOTIFY levelChanged)
};

#endif // DMXVALUE_H
