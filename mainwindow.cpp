#include "mainwindow.h"
#include <QDebug>
//#include "QEnttecPro/qenttecpro.h"
#include "qdmxlib/QDmxManager"
//#include "QDmxLib/include/qdmxlib/QDmxManager"
#include "qdmxlib/QDmxUsbDriver"


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  CreateActions();
  CreateMenubar();
  CreateToolBars();
  CreateDockWidgets();
  CreateCentralWidget();
  CreateStatusBar();

//  auto serialPort = new SerialPort(this);

//  auto enttectest = new EnttecDMXUSB();
//  auto interfaceTest = new InterfaceDMX(this);
//  qDebug() << interfaceTest->seConnecter();
//  qDebug() << QString::fromStdString(interfaceTest->getNom());
//  qDebug() << interfaceTest->setCanalDMX(1, 50);
//  qDebug() << interfaceTest->envoyer();

//  auto myEnttec = new QEnttecPro();
//  qDebug() << myEnttec->connect("/dev/ttyUSB0");
//  qDebug() << myEnttec->getVersionString();
//  myEnttec->setLevel(1, 99);
//  qDebug() << myEnttec->getLevel(1);

//  auto dmxManager = new QDmxManager(this);
//  QDmxManager::instance()->availableDevices();
//  for(auto d : QDmxManager::instance()->availableDevices())
//      {
////          QVERIFY(!d->isStarted());
//          qDebug() <<  d->name();
//      }
//  auto dmxManager = new QDmxManager(this);
//  auto test = QDmxManager::instance()->availableDrivers();
//  qDebug() << test;
//  auto test2 = QDmxManager::instance()->availableDevices();
//  qDebug() << test2;
  auto test  = QDmxManager::instance();
  test->init();
  qDebug() << test->availableDrivers();
  qDebug() << test->availableDevices();
  qDebug() << test->configLocation();
//  auto driver = new QDmxUsbDriver(test);
  qDebug() << test->patch("dummy", "dummy", 0, 0);
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

}

void MainWindow::CreateCentralWidget()
{

}

void MainWindow::CreateStatusBar()
{

}

