#include "dmxoutputwindow.h"

DmxOutputWindow::DmxOutputWindow(QWidget *parent)
  : QWidget(parent)
{
  setWindowFlags(Qt::Window
                 | Qt::WindowStaysOnTopHint);
}
