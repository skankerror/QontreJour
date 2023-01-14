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
//#include "QEnttecPro/qenttecpro.h"
#include "qdmxlib/QDmxManager"
#include "qdmxlib/QDmxUsbDriver"


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_tabWidget(new QTabWidget(this)),
    m_dmxManagerWidget(new DMXManagerWidget(this)),
    m_dmxManager(QDmxManager::instance()),
    m_grandMasterWidget(new GrandMasterWidget(this)),
    m_playbackWidget(new PlaybackWidget(this))
{
  CreateActions();
  CreateMenubar();
  CreateToolBars();
  CreateDockWidgets();
  CreateCentralWidget();
  CreateStatusBar();

//  TestingZone();
}

void MainWindow::TestingZone()
{
  // The QDmxManager is what you will use most of the time.
    // It is responsible for managing which DMX data goes to which DMX device
    // it provides patching methods to assign a physical device to a DMX universe
    // and read and write methods to work with DMX data

    // Please note that this the manager is implemented as a Singleton,
    // There's one and only one global instance of the manager that you can
    // retrieve using the following method anywhere in your program.
//    auto manager = QDmxManager::instance();
//    manager->init();

    // The manager also has methods to get what driver are available and
    // what devices within those drivers
//    qDebug() << "List all drivers:";
//    for(auto d : manager->availableDrivers())
//      qDebug() << "- " << d->name();

    // Let's say you want to use the art-net driver
    // you need to enable it first
//    auto driver = manager->driver("dmxusb");
//    Q_ASSERT(driver);

//    driver->setEnabled(true);

    // now the driver is enabled you can list all available devices
//    qDebug() << "List all devices for the dmxusb driver:";
//    for(auto d : driver->availableDevices())
//      qDebug() << "- " << d->name();

    // The art-net drivers (as all network based driver) creates one device per network interface
    // the name the device is the name of said interface.
    // Assuming you are on a unix machine, you can query the manager like so:
//    auto dev = manager->device("dmxusb", "OpenDMX");

//    Q_ASSERT(dev); // The methods return nullptr if it can't find the device

    // alternatively, you could have query the driver directly like this
  //  dev = driver->device("eth0");


    // then you need to patch the different input / output of your device to
    // a DMX universe. Let's just patch the first output to the first universe here.

    // note: this methods returns true on successful patch

//    manager->patch(QDmxManager::Output, /* Type of port to patch*/
//                   dev,                 /* device to patch */
//                   0,                   /* Port id on the device */
//                   0);                  /* Universe id */

    // let's also patch the first input port
  //  manager->patch(QDmxManager::Input, dev, 0, 0);

    // Now that things are patched, we can start playing with DMX

    // you can get notification of incoming dmx data by connecting the following
    // signal.
    // The 2 parameters are the universe id and the raw dmx data as a byte array
  //  QObject::connect(manager, &QDmxManager::inputDataChanged, [](quint8 universe, const QByteArray& dmxData)
  //  {
  //    qDebug() << "Received new data on universe" << universe;
  //    qDebug() << dmxData;
  //  });

    // Alternatively you can read the output directly using:
  //  QByteArray inputData = manager->readInputData(0 /* universe */);

    // let's do a dirty ramp up from 0 to 255
//    for(int i = 0; i < 255; i++)
//    {
//      manager->writeData(0,  /* Universe */
//                         0,  /* Channel */
//                         i); /* Value */

      // Please note: The channels indexing starts from 0 and ends at 511
      // So what usually people call DMX channel 1, is index 0 in this library
//    }

    // The writeData methods also have overloads that takes QByteArray argument
    // to modify multiple consicutive value in one call like so:
  //  QByteArray multiple;
  //  multiple.append(10);
  //  multiple.append(20);
  //  multiple.append(30);
  //  multiple.append(40);

    // The channels with index 100, 101, 102 and 103 (DMX channel 101, 102, 103 and 104 in the "real" world)
    // will be updated with the 4 values in the byte array
  //  manager->writeData(0, 100, multiple);

    // If you ever want to know what is currently being send you can also read
    // the output buffer:
//    QByteArray outputData = manager->readOutputData(0 /* universe */);
//    qDebug() << outputData;
    // Once you're done, before your application shutdown, don't forget to call
    // the teardown method
//    manager->teardown();

    //  auto test  = QDmxManager::instance();
    //  test->init();
    //  qDebug() << test->availableDrivers();
    //  qDebug() << test->availableDevices();
    //  qDebug() << test->configLocation();
    //  auto driver = new QDmxUsbDriver(test);
    //  qDebug() << test->patch("dummy", "dummy", 0, 0);
    //  test->
    //  test->teardown();


}



MainWindow::~MainWindow()
{
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
  auto leftDock = new QDockWidget(this);
  leftDock->setAllowedAreas(Qt::LeftDockWidgetArea/* | Qt::RightDockWidgetArea*/);
  leftDock->setWidget(m_grandMasterWidget);
  addDockWidget(Qt::LeftDockWidgetArea, leftDock);

  auto rightDock = new QDockWidget(this);
  rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
  rightDock->setWidget(m_playbackWidget);
  addDockWidget(Qt::RightDockWidgetArea, rightDock);
}

void MainWindow::CreateCentralWidget()
{
  m_tabWidget->addTab(m_dmxManagerWidget, "DMX Manager");
  setCentralWidget(m_tabWidget);
}

void MainWindow::CreateStatusBar()
{

}

