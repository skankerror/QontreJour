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

  explicit UniverseWidget(int t_universeID,
                            QWidget *parent = nullptr);
  ~UniverseWidget();

private :

  void CreateWidget();
  void CreateConnections();

private slots :

  void PopulateDevices(const QString &t_driverString = "dummy");
  void Connect();
  void disConnect();

private :

  QLabel *m_universeIDLabel;
  QComboBox *m_dmxDriversComboBox;
  QComboBox *m_dmxDevicesComboBox;
  QPushButton *m_dmxConnect;

  int m_universeID;

  bool m_isConnected;

};

#endif // UNIVERSEWIDGET_H