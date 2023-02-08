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
    m_dmxManager(DmxManager::instance()),
    m_dmxChannelTableWidget(new DmxValueTableWidget(this)),
    m_directChannelWidget(new DirectChannelWidget(this)),
    m_dmxUniverseWidgetContainerLayout(new QVBoxLayout()),
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
  auto dmxUniverseContainerWidget = createDmxUniverseContainerWidget();

  auto submasterWidget = new SubmasterWidget(this);
  setDirectChannelWidget(0); // set for first universe

  auto tabWidget = new QTabWidget(this);
  tabWidget->addTab(submasterWidget, "Submasters");
  tabWidget->addTab(m_directChannelWidget, "Direct Channels");
  tabWidget->addTab(dmxUniverseContainerWidget, "DMX Connections");

  setCentralWidget(tabWidget);
}

QWidget *MainWindow::createDmxUniverseContainerWidget()
{
  auto dmxUniverseContainerWidget = new QWidget(this);
  auto addDmxUniverseButton = new QPushButton("Add universe",
                                             dmxUniverseContainerWidget);
  auto removeDmxUniverseButton = new QPushButton("Remove Universe",
                                                dmxUniverseContainerWidget);
  auto buttonsLayout = new QHBoxLayout();
  buttonsLayout->addWidget(addDmxUniverseButton);
  buttonsLayout->addWidget(removeDmxUniverseButton);
  m_dmxUniverseWidgetContainerLayout->addLayout(buttonsLayout);

  // create first dmx manager widget for first universe
  addDmxUniverseWidget();

  dmxUniverseContainerWidget->setLayout(m_dmxUniverseWidgetContainerLayout);

  connect(addDmxUniverseButton,
          SIGNAL(clicked()),
          this,
          SLOT(addDmxUniverseWidget()));

  connect(removeDmxUniverseButton,
          SIGNAL(clicked()),
          this,
          SLOT(removeDmxUniverseWidget()));

  return dmxUniverseContainerWidget;
}

void MainWindow::setDirectChannelWidget(int t_universeID)
{
  auto rootChannel = m_L_dmxUniverseWidget.at(t_universeID)
      ->getDmxUniverse()
      ->getRootChannel();
  m_directChannelWidget->setRootValue(rootChannel);

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
  m_dmxChannelTableWidget->setRootValue(m_L_dmxUniverseWidget
                                        .first()
                                        ->getDmxUniverse()
                                        ->getRootChannel());
  m_dmxChannelTableWidget->setUniverseID(0);
  bottomDock->setAllowedAreas(Qt::BottomDockWidgetArea);
  bottomDock->setWidget(m_dmxChannelTableWidget);
  bottomDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::BottomDockWidgetArea, bottomDock);

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

}

void MainWindow::addDmxUniverseWidget()
{
  auto dmxUniverseWidget = new DmxUniverseWidget(m_universeCount,
                                               /*m_dmxUniverseContainerWidget*/this);

  m_L_dmxUniverseWidget.append(dmxUniverseWidget);
  m_dmxUniverseWidgetContainerLayout->addWidget(dmxUniverseWidget);

  m_universeCount++;

  emit universeCountChanged(m_universeCount);
}

void MainWindow::removeDmxUniverseWidget()
{
  if (m_L_dmxUniverseWidget.size() > 1) // we always keep one universe
  {
    // TODO : préserver l'univers quelque part !
    auto dmxUniverseWidget = m_L_dmxUniverseWidget.last();
    m_L_dmxUniverseWidget.removeLast();
    dmxUniverseWidget->deleteLater();

    emit universeCountChanged(--m_universeCount);

    // on remet le channel output widget au bon endroit
    dmxUniverseWidget = m_L_dmxUniverseWidget.first(); // we get first universe
    auto dmxUniverse = dmxUniverseWidget->getDmxUniverse();
    m_dmxChannelTableWidget->setUniverseID(m_universeCount - 1);
    m_dmxChannelTableWidget->setRootValue(dmxUniverse->getRootChannel());
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
