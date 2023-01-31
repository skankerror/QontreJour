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

#ifndef DMXUNIVERSE_H
#define DMXUNIVERSE_H

//#include <QObject>
//#include <QList>
#include "dmxvalue.h"
#include "../qontrejour.h"
#include "qdmxlib/QDmxManager"

class DmxUniverse
    : public QObject
{

  Q_OBJECT

public:

  // cstr
  explicit DmxUniverse(int t_universeID,
                       int t_outputCount = UNIVERSE_OUTPUT_COUNT_DEFAULT,
                       QObject *parent = nullptr);

  //destr
  ~DmxUniverse();

  // getters
  int getID() const { return m_ID; }
  int getOutputCount() const { return m_outputCount; }
  QDmxDevice* getDmxDevice() const { return m_dmxDevice; }
  bool isConnected() const { return m_isConnected; }
  QList<DmxValue *> getL_dmxChannel() const { return m_L_dmxChannel; }

  // setters
  void setID(int t_ID) { m_ID = t_ID; }
//  bool setOutputLevel(int t_outputID,
//                      int t_level);
  void setDmxDevice(QDmxDevice *t_dmxDevice) { m_dmxDevice = t_dmxDevice; }
  void setConnected(bool t_isConnected) { m_isConnected = t_isConnected; }


signals:

  void dmxOutputUpdateRequired(int, // output id
                               int); // level

private slots:

  void onRequestDmxUpdate(int t_ID,
                          quint8 t_level);

private:

  QList<DmxValue *> m_L_dmxOutput;
  QList<DmxValue *> m_L_dmxChannel;
  int m_ID;
  int m_outputCount;

  QDmxDevice *m_dmxDevice;
  bool m_isConnected;

  // root value, parent of all values created for this universe
  DmxValue *m_rootValue;

};

#endif // DMXUNIVERSE_H
