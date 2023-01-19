#ifndef SUBMASTERWIDGET_H
#define SUBMASTERWIDGET_H

#include <QWidget>
#include <QSlider>
#include "../qontrejour.h"
#include "../core/dmxchannelgroup.h"

class SubMasterWidget : public QWidget
{

  Q_OBJECT

public:

  explicit SubMasterWidget(QWidget *parent = nullptr);

signals:

private:

  QList<QSlider*> m_L_sliders;

};

#endif // SUBMASTERWIDGET_H
