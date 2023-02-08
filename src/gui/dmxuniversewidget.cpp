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

#include <QDebug>
#include "dmxuniversewidget.h"
#include "../qontrejour.h"


DmxUniverseWidget::DmxUniverseWidget(int t_universeID,
                                     QWidget *parent)
  : QWidget(parent),
    m_universeIDLabel(new QLabel(this)),
    m_dmxDriversComboBox(new QComboBox(this)),
    m_dmxDevicesComboBox(new QComboBox(this)),
    m_dmxConnect(new QPushButton(this)),
    m_isConnected(false),
    m_dmxUniverse(new DmxUniverse(t_universeID,
                                  UNIVERSE_OUTPUT_COUNT_DEFAULT,
                                  this)),
    m_universeID(t_universeID)
{
  CreateWidget();
  CreateConnections();
}

DmxUniverseWidget::~DmxUniverseWidget()
{
  if (m_isConnected)
  {
    disConnect();
  }
}

void DmxUniverseWidget::CreateWidget()
{
  auto layout = new QHBoxLayout();

  auto dmxManager = DmxManager::instance();
  m_dmxDriversComboBox->addItems(dmxManager->getAvailableDriversNames());

  PopulateDevices();

  m_universeIDLabel->setText(QString(tr("Universe %1").arg(m_universeID + 1)));
  m_dmxConnect->setText("Connect");

  layout->addWidget(m_universeIDLabel);
  layout->addWidget(m_dmxDriversComboBox);
  layout->addWidget(m_dmxDevicesComboBox);
  layout->addWidget(m_dmxConnect);
  setLayout(layout);

}

void DmxUniverseWidget::CreateConnections()
{
  QObject::connect(m_dmxDriversComboBox,
                   SIGNAL(currentTextChanged(QString)),
                   this,
                   SLOT(PopulateDevices(QString)));
  QObject::connect(m_dmxConnect,
                   SIGNAL(clicked(bool)),
                   this,
                   SLOT(Connect()));
}

void DmxUniverseWidget::PopulateDevices(const QString &t_driverString /* = "dummy" */)
{
  auto dmxManager = DmxManager::instance();

  m_dmxDevicesComboBox->clear();

  m_dmxDevicesComboBox->addItems(dmxManager->getAvailableDevicesNames(t_driverString));

  if (!m_dmxDevicesComboBox->count())
    m_dmxDevicesComboBox->addItem("No device");
}

void DmxUniverseWidget::Connect()
{
  // TODO: this if is really ugly, bad work !
  if (m_dmxDevicesComboBox->currentText() == "No device")
  {
    qWarning() << "no device to connect !";
  }
  else
  {
    auto driverString = m_dmxDriversComboBox->currentText();
    auto deviceString = m_dmxDevicesComboBox->currentText();
    if (DmxManager::instance()
        ->hwConnect(DmxManager::HwOutput,
                    driverString,
                    deviceString,
                    0, // first output...
                    m_universeID))
    {
      m_isConnected = true;
      m_dmxConnect->setText("Disconnect");

      QObject::disconnect(m_dmxConnect,
                          SIGNAL(clicked(bool)),
                          this,
                          SLOT(Connect()));

      QObject::connect(m_dmxConnect,
                       SIGNAL(clicked(bool)),
                       this,
                       SLOT(disConnect()));
    }
  }
}

void DmxUniverseWidget::disConnect()
{
  if (m_isConnected)
  {
    if (DmxManager::instance()
        ->hwDisconnect(m_universeID))
    {
      m_isConnected = false;
      m_dmxConnect->setText("Connect");

      QObject::disconnect(m_dmxConnect,
                          SIGNAL(clicked(bool)),
                          this,
                          SLOT(disConnect()));

      QObject::connect(m_dmxConnect,
                       SIGNAL(clicked(bool)),
                       this,
                       SLOT(Connect()));

    }
  }
}
