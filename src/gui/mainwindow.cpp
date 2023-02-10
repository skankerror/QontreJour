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


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_channelTableWidget(new ValueTableWidget(this)),
    m_directChannelWidget(new DirectChannelWidget(this)),
    m_universeWidgetContainerLayout(new QVBoxLayout())
{
  createCentralWidget();
  createDockWidgets();
}

MainWindow::~MainWindow()
{}

void MainWindow::createCentralWidget()
{
  auto dmxUniverseContainerWidget = createUniverseContainerWidget();

  auto submasterWidget = new SubmasterWidget(this);
  m_directChannelWidget->setDirectChannelUniverseID(0);
  auto tabWidget = new QTabWidget(this);
  tabWidget->addTab(submasterWidget, "Submasters");
  tabWidget->addTab(m_directChannelWidget, "Direct Channels");
  tabWidget->addTab(dmxUniverseContainerWidget, "DMX Connections");

  QMainWindow::setCentralWidget(tabWidget);
}

QWidget *MainWindow::createUniverseContainerWidget()
{
  auto universeContainerWidget = new QWidget(this);
  auto addUniverseButton = new QPushButton("Add universe",
                                           universeContainerWidget);
  auto removeUniverseButton = new QPushButton("Remove Universe",
                                              universeContainerWidget);
  auto buttonsLayout = new QHBoxLayout();
  buttonsLayout->addWidget(addUniverseButton);
  buttonsLayout->addWidget(removeUniverseButton);
  m_universeWidgetContainerLayout->addLayout(buttonsLayout);

  // create first dmx manager widget for first universe
  addUniverseWidget();

  universeContainerWidget->setLayout(m_universeWidgetContainerLayout);

  connect(addUniverseButton,
          SIGNAL(clicked()),
          this,
          SLOT(addUniverseWidget()));

  connect(removeUniverseButton,
          SIGNAL(clicked()),
          this,
          SLOT(removeUniverseWidget()));

  return universeContainerWidget;
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
  m_channelTableWidget->setRootValueFromUid(0);
  m_channelTableWidget->setUniverseID(0);
  bottomDock->setAllowedAreas(Qt::BottomDockWidgetArea);
  bottomDock->setWidget(m_channelTableWidget);
  bottomDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::BottomDockWidgetArea, bottomDock);

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

void MainWindow::addUniverseWidget()
{
  auto universeWidget = new UniverseWidget(m_L_universeWidget.size(),
                                           this);
  m_L_universeWidget.append(universeWidget);
  m_universeWidgetContainerLayout->addWidget(universeWidget);

}

void MainWindow::removeUniverseWidget()
{
  if (m_L_universeWidget.size() > 1) // we always keep one universe
  {
    auto universeWidget = m_L_universeWidget.last();
    m_L_universeWidget.removeLast();
    universeWidget->deleteLater();

    // TODO : gérer ça via manager, dire au channel table d'aller
    // upgrader son universe ?
    // ou bien faire un slot qui réagit à un signal du manager ?
  }
}
