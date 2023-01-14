#ifndef QABSTRACTMIDICONTROLERWIDGET_H
#define QABSTRACTMIDICONTROLERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QVector>

class QAbstractMidiControlerWidget :
    public QWidget
{

  Q_OBJECT

public :

  explicit QAbstractMidiControlerWidget(QWidget *parent = nullptr);
  virtual ~QAbstractMidiControlerWidget(){}

protected :

  virtual void drawWidget() = 0;

signals:

protected :

  QVector<QPushButton *> m_v_pushButtons;
  QVector<QSlider *> m_v_sliders;
  QVector<QLabel *> m_v_labels;

};

#endif // QABSTRACTMIDICONTROLERWIDGET_H
