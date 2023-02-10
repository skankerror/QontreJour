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

#ifndef UNIVERSEWIDGET_H
#define UNIVERSEWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include "../core/dmxmanager.h"

class UniverseWidget
    : public QWidget
{

  Q_OBJECT

public :

  explicit UniverseWidget(uid t_uid,
                          QWidget *parent = nullptr);

  ~UniverseWidget();

  uid getUniverseID() const{ return m_universeID; }

private :

  void CreateWidget();
  void CreateConnections();

public slots:

  void setUniverseID(uid t_universeID){ m_universeID = t_universeID; }

private slots :

  void PopulateDevices(const QString &t_driverString = "dummy");
  void Connect();
  void disConnect();

private :

  QLabel *m_universeIDLabel;
  QComboBox *m_dmxDriversComboBox;
  QComboBox *m_dmxDevicesComboBox;
  QPushButton *m_dmxConnect;

  uid m_universeID;

  bool m_isConnected;

};

#endif // UNIVERSEWIDGET_H
