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
#include <QWindow>
#include "qdmxlib/QDmxManager"
#include "qdmxlib/QDmxUsbDriver"


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_tabWidget(new QTabWidget(this)),
    m_dmxManager(QDmxManager::instance()),
    m_grandMasterWidget(new GrandMasterWidget(this)),
    m_playbackWidget(new PlaybackWidget(this)),
    m_sequencerWidget(new SequencerWidget(this)),
    m_dmxChannelOutputWidget(new DmxChannelOutputWidget(this)),
//    m_submasterWidget(new SubMasterWidget(this)),
    m_directChannelWidget(new DirectChannelWidget(this)),
    m_dmxManagerContainerWidget(new QWidget(this)),
    m_dmxManagerContainerLayout(new QVBoxLayout()),
    m_addDmxManagerWidgetButton(new QPushButton(m_dmxManagerContainerWidget)),
    m_removeDmxManagerWidgetButton(new QPushButton(m_dmxManagerContainerWidget)),
    m_universeCount(0)
//    m_dmxOutputWindow(new QWindow())
{
//  m_dmxOutputWindow->show();
//  CreateActions();
//  CreateMenubar();
//  CreateToolBars();
  createDmxManagerContainerWidget();
  CreateDockWidgets();
  CreateCentralWidget();
//  CreateStatusBar();

  createConnections();

}

MainWindow::~MainWindow()
{}


void MainWindow::addDmxManagerWidget()
{
  auto dmxManagerWidget = new DmxManagerWidget(m_universeCount,
                                               m_dmxManagerContainerWidget);

  m_L_dmxManagerWidget.append(dmxManagerWidget);
  m_dmxManagerContainerLayout->addWidget(dmxManagerWidget);

  // we get his universe to get *DmxChannel to set
  // model and delegatefor channel view
  auto dmxUniverse = dmxManagerWidget->getDmxUniverse();
  auto L_dmxChannel = dmxUniverse->getL_dmxChannel();
  QList<DmxValue *> L_dmxValue;

  // we connect to update views
//  for (const auto &item : std::as_const(L_dmxChannel))
  for (const auto &item : std::as_const(L_dmxChannel))
  {
    connect(item,
            SIGNAL(levelChanged(DmxValue::SignalSenderType,quint8)),
            m_dmxChannelOutputWidget,
            SLOT(repaintTableView()));

    DmxValue *value = item;
    L_dmxValue.append(value);
  }

//  m_dmxChannelOutputWidget->setL_dmxChannel(L_dmxChannel);
  m_dmxChannelOutputWidget->setL_dmxValue(L_dmxValue);
  m_dmxChannelOutputWidget->setUniverseID(m_universeCount); // count has not been ++ yet

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
              m_dmxChannelOutputWidget,
              SLOT(repaintTableView()));
      DmxValue *value = item;
      L_dmxValue.append(value);
    }
//    m_dmxChannelOutputWidget->setL_dmxChannel(L_dmxChannel);
    m_dmxChannelOutputWidget->setL_dmxValue(L_dmxValue);
    m_dmxChannelOutputWidget->setUniverseID(m_universeCount - 1);
  }
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
    L_directChannelSlider.append(directChannelSlider);
  }
  // les donner aux widgets
  m_directChannelWidget->setL_sliders(L_directChannelSlider);
  m_directChannelWidget->populateWidget();

}

void MainWindow::CreateActions()
{

}

void MainWindow::CreateMenubar()
{

}

void MainWindow::CreateToolBars()
{

}

void MainWindow::CreateDockWidgets()
{
//  auto leftDock = new QDockWidget(this);
//  leftDock->setAllowedAreas(Qt::LeftDockWidgetArea);
//  leftDock->setWidget(m_grandMasterWidget);
//  leftDock->setFeatures(QDockWidget::DockWidgetFloatable);
//  addDockWidget(Qt::LeftDockWidgetArea, leftDock);

  auto rightDock = new QDockWidget(this);
  auto rightDockWidget = new QWidget(rightDock);
  auto layout = new QVBoxLayout();
  layout->addWidget(m_grandMasterWidget);
  layout->addWidget(m_playbackWidget);
  rightDockWidget->setLayout(layout);
  rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
//  rightDock->setWidget(m_playbackWidget);
  rightDock->setWidget(rightDockWidget);
  rightDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::RightDockWidgetArea, rightDock);

  auto topDock = new QDockWidget(this);
  topDock->setAllowedAreas(Qt::TopDockWidgetArea);
  topDock->setWidget(m_sequencerWidget);
  topDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::TopDockWidgetArea, topDock);

  auto bottomDock = new QDockWidget();
  bottomDock->setAllowedAreas(Qt::BottomDockWidgetArea);
  bottomDock->setWidget(m_dmxChannelOutputWidget);
  bottomDock->setFeatures(QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::BottomDockWidgetArea, bottomDock);

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

}

void MainWindow::createDmxManagerContainerWidget()
{
  m_addDmxManagerWidgetButton->setText("Add universe");
  m_removeDmxManagerWidgetButton->setText("Remove universe");
  auto buttonsLayout = new QHBoxLayout();
  buttonsLayout->addWidget(m_addDmxManagerWidgetButton);
  buttonsLayout->addWidget(m_removeDmxManagerWidgetButton);
  m_dmxManagerContainerLayout->addLayout(buttonsLayout);

  // create first dmx manager widget for first universe
  addDmxManagerWidget();

  m_dmxManagerContainerWidget->setLayout(m_dmxManagerContainerLayout);
}

void MainWindow::CreateCentralWidget()
{
//  // create direct channel widget
//  auto L_dmxChannel = m_L_dmxManagerWidget.at(0)
//      ->getDmxUniverse()
//      ->getL_dmxChannel();
//  auto L_directChannelSlider = QList<DmxValueSlider *>();
//  for (const auto &item : std::as_const(L_dmxChannel))
//  {
//    auto directChannelSlider = new DmxValueSlider(item);
//    L_directChannelSlider.append(directChannelSlider);
//  }
//  // les donner aux widgets
//  m_directChannelWidget->setL_sliders(L_directChannelSlider);
//  m_directChannelWidget->populateWidget();
  setDirectChannelWidget(0); // set for first universe

  m_tabWidget->addTab(m_directChannelWidget, "Direct Channels");
  m_tabWidget->addTab(m_dmxManagerContainerWidget, "DMX Connections");
//  m_tabWidget->setMaximumSize(1024, 600);

  setCentralWidget(m_tabWidget);

//  resize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
//  setGeometry(0, 0, 1280, 720);
}

void MainWindow::CreateStatusBar()
{

}

void MainWindow::createConnections()
{
  connect(m_addDmxManagerWidgetButton,
          SIGNAL(clicked()),
          this,
          SLOT(addDmxManagerWidget()));

  connect(m_removeDmxManagerWidgetButton,
          SIGNAL(clicked()),
          this,
          SLOT(removeDmxManagerWidget()));
  connect(this,
          SIGNAL(universeCountChanged(int)),
          m_dmxChannelOutputWidget,
          SLOT(onUniverseCountChanged(int)));

  connect(m_dmxChannelOutputWidget,
          SIGNAL(askForUniverseChanged(int)),
          this,
          SLOT(setDirectChannelWidget(int)));

}

//void MainWindow::TestingZone()
//{
//  // The QDmxManager is what you will use most of the time.
//    // It is responsible for managing which DMX data goes to which DMX device
//    // it provides patching methods to assign a physical device to a DMX universe
//    // and read and write methods to work with DMX data

//    // Please note that this the manager is implemented as a Singleton,
//    // There's one and only one global instance of the manager that you can
//    // retrieve using the following method anywhere in your program.
////    auto manager = QDmxManager::instance();
////    manager->init();

//    // The manager also has methods to get what driver are available and
//    // what devices within those drivers
////    qDebug() << "List all drivers:";
////    for(auto d : manager->availableDrivers())
////      qDebug() << "- " << d->name();

//    // Let's say you want to use the art-net driver
//    // you need to enable it first
////    auto driver = manager->driver("dmxusb");
////    Q_ASSERT(driver);

////    driver->setEnabled(true);

//    // now the driver is enabled you can list all available devices
////    qDebug() << "List all devices for the dmxusb driver:";
////    for(auto d : driver->availableDevices())
////      qDebug() << "- " << d->name();

//    // The art-net drivers (as all network based driver) creates one device per network interface
//    // the name the device is the name of said interface.
//    // Assuming you are on a unix machine, you can query the manager like so:
////    auto dev = manager->device("dmxusb", "OpenDMX");

////    Q_ASSERT(dev); // The methods return nullptr if it can't find the device

//    // alternatively, you could have query the driver directly like this
//  //  dev = driver->device("eth0");


//    // then you need to patch the different input / output of your device to
//    // a DMX universe. Let's just patch the first output to the first universe here.

//    // note: this methods returns true on successful patch

////    manager->patch(QDmxManager::Output, /* Type of port to patch*/
////                   dev,                 /* device to patch */
////                   0,                   /* Port id on the device */
////                   0);                  /* Universe id */

//    // let's also patch the first input port
//  //  manager->patch(QDmxManager::Input, dev, 0, 0);

//    // Now that things are patched, we can start playing with DMX

//    // you can get notification of incoming dmx data by connecting the following
//    // signal.
//    // The 2 parameters are the universe id and the raw dmx data as a byte array
//  //  QObject::connect(manager, &QDmxManager::inputDataChanged, [](quint8 universe, const QByteArray& dmxData)
//  //  {
//  //    qDebug() << "Received new data on universe" << universe;
//  //    qDebug() << dmxData;
//  //  });

//    // Alternatively you can read the output directly using:
//  //  QByteArray inputData = manager->readInputData(0 /* universe */);

//    // let's do a dirty ramp up from 0 to 255
////    for(int i = 0; i < 255; i++)
////    {
////      manager->writeData(0,  /* Universe */
////                         0,  /* Channel */
////                         i); /* Value */

//      // Please note: The channels indexing starts from 0 and ends at 511
//      // So what usually people call DMX channel 1, is index 0 in this library
////    }

//    // The writeData methods also have overloads that takes QByteArray argument
//    // to modify multiple consicutive value in one call like so:
//  //  QByteArray multiple;
//  //  multiple.append(10);
//  //  multiple.append(20);
//  //  multiple.append(30);
//  //  multiple.append(40);

//    // The channels with index 100, 101, 102 and 103 (DMX channel 101, 102, 103 and 104 in the "real" world)
//    // will be updated with the 4 values in the byte array
//  //  manager->writeData(0, 100, multiple);

//    // If you ever want to know what is currently being send you can also read
//    // the output buffer:
////    QByteArray outputData = manager->readOutputData(0 /* universe */);
////    qDebug() << outputData;
//    // Once you're done, before your application shutdown, don't forget to call
//    // the teardown method
////    manager->teardown();

//    //  auto test  = QDmxManager::instance();
//    //  test->init();
//    //  qDebug() << test->availableDrivers();
//    //  qDebug() << test->availableDevices();
//    //  qDebug() << test->configLocation();
//    //  auto driver = new QDmxUsbDriver(test);
//    //  qDebug() << test->patch("dummy", "dummy", 0, 0);
//    //  test->
//    //  test->teardown();


//}
