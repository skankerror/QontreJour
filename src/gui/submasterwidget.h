#ifndef SUBMASTERWIDGET_H
#define SUBMASTERWIDGET_H

#include <QWidget>
#include <QSlider>
#include "../qontrejour.h"

class SubMasterWidget : public QWidget
{

  Q_OBJECT

public:

  explicit SubMasterWidget(QWidget *parent = nullptr);

signals:

private:

  QList<QSlider*> l_m_sliders;

};

#endif // SUBMASTERWIDGET_H
