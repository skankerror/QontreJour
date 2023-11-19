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
#include "../core/dmxmanager.h"
#include "../gui/keypadwidget.h"


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_channelTableWidget(new ValueTableWidget(this)),
    m_directChannelWidget(new DirectChannelWidget(this)),
    m_universeWidgetContainerLayout(new QVBoxLayout())
{
  MANAGER;

  createCentralWidget();
  createDockWidgets();
}

MainWindow::~MainWindow()
{}

void MainWindow::createCentralWidget()
{
  auto dmxUniverseContainerWidget = createUniverseContainerWidget();

  auto submasterWidget = new SubmasterWidget(this);
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

  auto sequencerWidget = new SequencerWidget(MANAGER->getL_sequence().at(0),
                                             this);

  auto topDock = new QDockWidget(this);
  topDock->setAllowedAreas(Qt::TopDockWidgetArea);
  topDock->setWidget(sequencerWidget);
  topDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::TopDockWidgetArea, topDock);

  auto bottomDock = new QDockWidget();
//  m_channelTableWidget->setRootValue(MANAGER->getRootChannel());
  bottomDock->setAllowedAreas(Qt::BottomDockWidgetArea);
  bottomDock->setWidget(m_channelTableWidget);
  bottomDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::BottomDockWidgetArea, bottomDock);

  auto manager = MANAGER;
  auto keypadWidget = new KeypadWidget(this);
  connect(keypadWidget,
          SIGNAL(buttonClicked(KeypadButton)),
          manager,
          SLOT(keypadToInterpreter(KeypadButton)));

  auto grandMasterWidget = new GrandMasterWidget(this);
  auto playbackWidget = new PlaybackWidget(this);
  connect(playbackWidget,
          SIGNAL(buttonClicked(PlayBackButton)),
          manager,
          SLOT(playBackToEngine(PlayBackButton)));

  auto rightDock = new QDockWidget(this);
  auto rightDockWidget = new QWidget(rightDock);
  auto layout = new QHBoxLayout();
  layout->addWidget(keypadWidget);
  layout->addWidget(playbackWidget);
  layout->addWidget(grandMasterWidget);
  rightDockWidget->setLayout(layout);
  rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
  rightDock->setWidget(rightDockWidget);
  rightDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::RightDockWidgetArea, rightDock);


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
