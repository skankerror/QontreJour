#ifndef DMXOUTPUTWINDOW_H
#define DMXOUTPUTWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include "../core/dmxoutput.h"

class DmxOutputWindow
    : public QWidget
{

  Q_OBJECT

public:

  DmxOutputWindow(QWidget *parent = nullptr);

private:

  QList<DmxOutput *> m_L_dmxOutput;
  int m_universeID;

  QSpinBox *m_universeSpinBox;


};

#endif // DMXOUTPUTWINDOW_H
