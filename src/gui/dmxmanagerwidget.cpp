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
#include "dmxmanagerwidget.h"
#include "../qontrejour.h"


DmxManagerWidget::DmxManagerWidget(QWidget *parent)
  : QWidget(parent),
    m_dmxDriversComboBox(new QComboBox(this)),
    m_dmxDevicesComboBox(new QComboBox(this)),
    m_dmxConnect(new QPushButton(this)),
    m_isConnected(false),
    m_dmxUniverse(new DmxUniverse(UNIVERSE_OUTPUT_COUNT_DEFAULT,
                                  this)),
    m_universeID(m_dmxUniverse->getID())
{
  CreateWidget();
  CreateConnections();
}

void DmxManagerWidget::CreateWidget()
{
  auto layout = new QHBoxLayout();

  m_dmxManager = QDmxManager::instance();
  m_dmxManager->init();
  for(auto d : m_dmxManager->availableDrivers())
  {
    m_dmxDriversComboBox->addItem(d->name());
  }

  PopulateDevices();

  m_dmxConnect->setText("Connect");

  layout->addWidget(m_dmxDriversComboBox);
  layout->addWidget(m_dmxDevicesComboBox);
  layout->addWidget(m_dmxConnect);
  setLayout(layout);

}

void DmxManagerWidget::CreateConnections()
{
  connect(m_dmxDriversComboBox,
          SIGNAL(currentTextChanged(QString)),
          this,
          SLOT(PopulateDevices(QString)));
  connect(m_dmxConnect,
          SIGNAL(clicked(bool)),
          this,
          SLOT(Connect()));
}

void DmxManagerWidget::PopulateDevices(const QString &t_driverString /* = "dummy" */)
{
  m_dmxDriver = m_dmxManager->driver(t_driverString);
  Q_ASSERT(m_dmxDriver);
  m_dmxDriver->setEnabled(true);

  m_dmxDevicesComboBox->clear();

  for(auto d : m_dmxDriver->availableDevices())
  {
    m_dmxDevicesComboBox->addItem(d->name());
  }

  if (!m_dmxDevicesComboBox->count())
    m_dmxDevicesComboBox->addItem("No device");
}

void DmxManagerWidget::Connect()
{
  m_dmxDevice = m_dmxManager->device(m_dmxDriversComboBox->currentText(),
                                     m_dmxDevicesComboBox->currentText());

  // TODO: this if is really ugly, bad work !
  if (m_dmxDevicesComboBox->currentText() == "No device")
  {
    qDebug() << "no device to connect !";
  }
  else
  {
    if(m_dmxManager->patch(QDmxManager::Output,
                        m_dmxDevice,
                        0, // TODO : handle several output ports
                        m_universeID,
                        QDmxManager::HTP)) // TODO : voir ça...
    {
      m_isConnected = true;
      m_dmxUniverse->setConnected(true);
      m_dmxUniverse->setDmxDevice(m_dmxDevice);
    }
  }
}

void DmxManagerWidget::disconnect()
{

}

