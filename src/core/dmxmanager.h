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

#ifndef DMXMANAGER_H
#define DMXMANAGER_H

#include <QObject>
#include "qdmxlib/QDmxManager"
#include "dmxvalue.h"
#include "dmxscene.h"
#include "dmxuniverse.h"


class DmxManager
    : public QObject
{

  Q_OBJECT

public :

  enum HwPortType
  {
    HwInput,
    HwOutput
  }; Q_ENUM(HwPortType)


  static DmxManager *instance();

  ~DmxManager();

  QStringList getAvailableDriversNames() const;
  QStringList getAvailableDevicesNames(const QString &t_driverString);
  int getSequenceCount() const{ return m_L_rootScene.size(); }

  bool createUniverse(uid t_universeID);
  bool createSequence();
  int getUniverseCount() const{ return m_L_universe.size() ;}

  bool hwConnect(DmxManager::HwPortType t_type,
                 QString &t_driver,
                 QString &t_device,
                 quint8 t_port,
                 uid t_ID);

  bool hwDisconnect(uid t_ID);

private :

  explicit DmxManager(QObject *parent = nullptr);
  QList<QDmxDriver *> getAvailableDrivers() const;
  QList<QDmxDevice *> getAvailableDevices(const QString &t_driverString);

signals :

private :

  QDmxManager *m_hwManager;

  QList<DmxUniverse *> m_L_universe;
  QList<DmxScene *> m_L_rootScene;
  DmxValue *m_rootChannelGroup;

};

#endif // DMXMANAGER_H
