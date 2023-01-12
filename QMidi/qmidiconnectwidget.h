#ifndef QMIDICONNECTWIDGET_H
#define QMIDICONNECTWIDGET_H

#include <QPushButton>
#include <QWidget>
#include "qmidiin.h"
#include "qmidiout.h"

// créer 3 classes pour les 3 types de boutons
// héritées de qpushbutton

class QMidiConnectInWidget :
    public QWidget
{

  Q_OBJECT

public :

  explicit QMidiConnectInWidget(QMidiIn *t_midiIn,
                                QWidget *parent = nullptr);


  QMidiIn *midiIn() const{ return m_midiIn; }
  void setMidiIn(QMidiIn *t_midiIn){ m_midiIn = t_midiIn; }

signals :

private :

  QMidiIn *m_midiIn;
  QPushButton *m_connectMidiButton;
};



class QMidiConnectOutWidget :
    public QWidget
{

  Q_OBJECT

public :

  explicit QMidiConnectOutWidget(QMidiOut *t_midiOut,
                                 QWidget *parent = nullptr);

  QMidiOut *midiOut() const{ return m_midiOut; }
  void setMidiOut(QMidiOut *t_midiOut){ m_midiOut = t_midiOut; }

signals :

private :

  QMidiOut *m_midiOut;
  QPushButton *m_connectMidiButton;
};

#endif // QMIDICONNECTWIDGET_H
