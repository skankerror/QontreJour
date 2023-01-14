#ifndef GRANDMASTERWIDGET_H
#define GRANDMASTERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>

class GrandMasterWidget : public QWidget
{

  Q_OBJECT

public:

  explicit GrandMasterWidget(QWidget *parent = nullptr);

signals:

private:

  QSlider *m_grandMasterslider;
  QPushButton *m_blackOutButton;
  QLabel *m_grandMasterLabel;

};

#endif // GRANDMASTERWIDGET_H
