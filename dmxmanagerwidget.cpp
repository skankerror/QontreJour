#include <QDebug>
#include "dmxmanagerwidget.h"

DMXManagerWidget::DMXManagerWidget(QWidget *parent)
  : QWidget(parent),
    m_dmxDriversComboBox(new QComboBox(this)),
    m_dmxDevicesComboBox(new QComboBox(this)),
    m_dmxConnect(new QPushButton(this))
{
  CreateWidget();
  CreateConnections();
}

void DMXManagerWidget::CreateWidget()
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

void DMXManagerWidget::CreateConnections()
{
  connect(m_dmxDriversComboBox,
          SIGNAL(currentTextChanged(QString)),
          this,
          SLOT(PopulateDevices(QString)));
}

void DMXManagerWidget::PopulateDevices(const QString &t_driverString /* = "dummy" */)
{
//  m_dmxDriver = m_dmxManager->driver(m_dmxDriversComboBox->currentText());
  m_dmxDriver = m_dmxManager->driver(t_driverString);
//  qDebug() << m_dmxDriversComboBox->currentText();
  Q_ASSERT(m_dmxDriver);
  m_dmxDriver->setEnabled(true);

  m_dmxDevicesComboBox->clear();

  for(auto d : m_dmxDriver->availableDevices())
  {
    m_dmxDevicesComboBox->addItem(d->name());
  }
}
