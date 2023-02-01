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

#include "mainwindow.h"
#include <QDebug>
#include <QDockWidget>
#include "qdmxlib/QDmxManager"
#include "qdmxlib/QDmxUsbDriver"


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_dmxManager(QDmxManager::instance()),
    m_dmxChannelTableWidget(new DmxValueTableWidget(this)),
    m_directChannelWidget(new DirectChannelWidget(this)),
    m_dmxManagerContainerLayout(new QVBoxLayout()),
    m_universeCount(0)
{
  createCentralWidget();
  createDockWidgets();

  createConnections();
}

MainWindow::~MainWindow()
{}

void MainWindow::createCentralWidget()
{
  auto dmxManagerContainerWidget = createDmxManagerContainerWidget();

  auto submasterWidget = createSubmasterWidget();
  setDirectChannelWidget(0); // set for first universe

  auto tabWidget = new QTabWidget(this);
  tabWidget->addTab(submasterWidget, "Submasters");
  tabWidget->addTab(m_directChannelWidget, "Direct Channels");
  tabWidget->addTab(dmxManagerContainerWidget, "DMX Connections");

  setCentralWidget(tabWidget);
}

QWidget *MainWindow::createDmxManagerContainerWidget()
{
  auto dmxManagerContainerWidget = new QWidget(this);
  auto addDmxManagerButton = new QPushButton("Add universe",
                                                dmxManagerContainerWidget);
  auto removeDmxManagerButton = new QPushButton("Remove Universe",
                                                dmxManagerContainerWidget);
  auto buttonsLayout = new QHBoxLayout();
  buttonsLayout->addWidget(addDmxManagerButton);
  buttonsLayout->addWidget(removeDmxManagerButton);
  m_dmxManagerContainerLayout->addLayout(buttonsLayout);

  // create first dmx manager widget for first universe
  addDmxManagerWidget();

  dmxManagerContainerWidget->setLayout(m_dmxManagerContainerLayout);

  connect(addDmxManagerButton,
          SIGNAL(clicked()),
          this,
          SLOT(addDmxManagerWidget()));

  connect(removeDmxManagerButton,
          SIGNAL(clicked()),
          this,
          SLOT(removeDmxManagerWidget()));

  return dmxManagerContainerWidget;
}

// NOTE : for the moment this method is here for testing,
// otherwise we can create it elsewhere
SubmasterWidget *MainWindow::createSubmasterWidget()
{
  auto submasterWidget = new SubmasterWidget(this);

// NOTE : for testing purposes we create some channel group
  auto L_dmxChannel = m_L_dmxManagerWidget.at(0)
      ->getDmxUniverse()
      ->getL_dmxChannel();
  auto L_submasterSlider = QList<SubmasterSlider *>();


  for (int i = 0; i < 60; i++)
  {
    auto channelGroup = new DmxValue(DmxValue::ChannelGroup);
    channelGroup->setID(i);

    auto dmxChannel1 = L_dmxChannel.at(i + 10);
    quint8 value1 = i +100;
    channelGroup->addChild(dmxChannel1);
    channelGroup->setStoredLevel(0, value1);
    auto dmxChannel2 = L_dmxChannel.at(i + 100);
    quint8 value2 = i + 150;
    channelGroup->addChild(dmxChannel2);
    channelGroup->setStoredLevel(1 , value2);


    auto slider = new SubmasterSlider(channelGroup,
                                      submasterWidget);
    L_submasterSlider.append(slider);
  }
  submasterWidget->setL_sliders(L_submasterSlider);
  submasterWidget->populateWidget();

  return submasterWidget;
}

void MainWindow::setDirectChannelWidget(int t_universeID)
{
  auto L_dmxChannel = m_L_dmxManagerWidget.at(t_universeID)
      ->getDmxUniverse()
      ->getL_dmxChannel();
  auto L_directChannelSlider = QList<DmxValueSlider *>();
  for (const auto &item : std::as_const(L_dmxChannel))
  {
    auto directChannelSlider = new DmxValueSlider(item,
                                                  m_directChannelWidget);
    connect(item,
            SIGNAL(blockDirectChannelSlider(quint8)),
            directChannelSlider,
            SLOT(unMoveSlider(quint8)));
    L_directChannelSlider.append(directChannelSlider);
  }

  // les donner aux widget
  m_directChannelWidget->setL_sliders(L_directChannelSlider);
  m_directChannelWidget->populateWidget();

}

void MainWindow::createDockWidgets()
{
  auto grandMasterWidget = new GrandMasterWidget(this);
  auto playbackWidget = new PlaybackWidget(this);
  auto sequencerWidget = new SequencerWidget(this);

  auto rightDock = new QDockWidget(this);
  auto rightDockWidget = new QWidget(rightDock);
  auto layout = new QVBoxLayout();
  layout->addWidget(grandMasterWidget);
  layout->addWidget(playbackWidget);
  rightDockWidget->setLayout(layout);
  rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
  rightDock->setWidget(rightDockWidget);
  rightDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::RightDockWidgetArea, rightDock);

  auto topDock = new QDockWidget(this);
  topDock->setAllowedAreas(Qt::TopDockWidgetArea);

  topDock->setWidget(sequencerWidget);
  topDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::TopDockWidgetArea, topDock);

  auto bottomDock = new QDockWidget();
  bottomDock->setAllowedAreas(Qt::BottomDockWidgetArea);
  bottomDock->setWidget(m_dmxChannelTableWidget);
  bottomDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::BottomDockWidgetArea, bottomDock);

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

}

void MainWindow::addDmxManagerWidget()
{
  auto dmxManagerWidget = new DmxManagerWidget(m_universeCount,
                                               /*m_dmxManagerContainerWidget*/this);

  m_L_dmxManagerWidget.append(dmxManagerWidget);
  m_dmxManagerContainerLayout->addWidget(dmxManagerWidget);

  // we get his universe to get *DmxChannel to set
  // model and delegatefor channel view
  auto dmxUniverse = dmxManagerWidget->getDmxUniverse();
  auto L_dmxChannel = dmxUniverse->getL_dmxChannel();
  QList<DmxValue *> L_dmxValue;

  // we connect to update views
  for (const auto &item : std::as_const(L_dmxChannel))
  {
    connect(item,
            SIGNAL(levelChanged(DmxValue::SignalSenderType,quint8)),
            m_dmxChannelTableWidget,
            SLOT(repaintTableView()));

    DmxValue *value = item;
    L_dmxValue.append(value);
  }

  m_dmxChannelTableWidget->setL_dmxValue(L_dmxValue);
  m_dmxChannelTableWidget->setUniverseID(m_universeCount); // count has not been ++ yet

  m_universeCount++;

  emit universeCountChanged(m_universeCount);

}

void MainWindow::removeDmxManagerWidget()
{
  if (m_L_dmxManagerWidget.size() > 1) // we always keep one universe
  {
    auto dmxManagerWidget = m_L_dmxManagerWidget.last();
    m_L_dmxManagerWidget.removeLast();
    dmxManagerWidget->deleteLater();

    emit universeCountChanged(--m_universeCount);

    // on remet le channel output widget au bon endroit
    dmxManagerWidget = m_L_dmxManagerWidget.first(); // we get first universe
    auto dmxUniverse = dmxManagerWidget->getDmxUniverse();
    auto L_dmxChannel = dmxUniverse->getL_dmxChannel();
    QList<DmxValue *> L_dmxValue;

    for (const auto &item : std::as_const(L_dmxChannel))
    {
      connect(item,
              SIGNAL(levelChanged(int,quint8)),
              m_dmxChannelTableWidget,
              SLOT(repaintTableView()));
      DmxValue *value = item;
      L_dmxValue.append(value);
    }
    m_dmxChannelTableWidget->setL_dmxValue(L_dmxValue);
    m_dmxChannelTableWidget->setUniverseID(m_universeCount - 1);
  }
}


void MainWindow::createConnections()
{
  connect(this,
          SIGNAL(universeCountChanged(int)),
          m_dmxChannelTableWidget,
          SLOT(onUniverseCountChanged(int)));

  connect(m_dmxChannelTableWidget,
          SIGNAL(askForUniverseChanged(int)),
          this,
          SLOT(setDirectChannelWidget(int)));

}
