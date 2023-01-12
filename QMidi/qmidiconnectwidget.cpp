#include "qmidiconnectwidget.h"
#include <QComboBox>
#include <QVBoxLayout>


QMidiConnectInWidget::QMidiConnectInWidget(QMidiIn *t_midiIn,
                                           QWidget *parent) :
  QWidget(parent),
  m_midiIn(t_midiIn),
  m_connectMidiButton(new QPushButton(this))
{
  auto layout = new QVBoxLayout();
  auto midiPortsComboBox = new QComboBox(this);
  auto refreshPortsButton = new QPushButton(this);

  m_connectMidiButton->setText("Connect");
  m_connectMidiButton->setFixedWidth(100);
  refreshPortsButton->setText("Refresh");

  layout->addWidget(m_connectMidiButton);
  layout->addWidget(midiPortsComboBox);
  layout->addWidget(refreshPortsButton);

  // faire connections

  setLayout(layout);
}

QMidiConnectOutWidget::QMidiConnectOutWidget(QMidiOut *t_midiOut,
                                             QWidget *parent) :
  QWidget(parent),
  m_midiOut(t_midiOut)
{
  auto layout = new QVBoxLayout();
  auto midiPortsComboBox = new QComboBox(this);
  auto refreshPortsButton = new QPushButton(this);

  m_connectMidiButton->setText("Connect");
  m_connectMidiButton->setFixedWidth(100);
  refreshPortsButton->setText("Refresh");

  layout->addWidget(m_connectMidiButton);
  layout->addWidget(midiPortsComboBox);
  layout->addWidget(refreshPortsButton);

  // faire connections

  setLayout(layout);
}
