#ifndef DMXMANAGERWIDGET_H
#define DMXMANAGERWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include "qdmxlib/QDmxManager"

class DMXManagerWidget : public QWidget
{

  Q_OBJECT

public:

  explicit DMXManagerWidget(QWidget *parent = nullptr);

private:

  void CreateWidget();
  void CreateConnections();

signals:

private slots:

  void PopulateDevices(const QString &t_driverString = "dummy");

private:

  QComboBox *m_dmxDriversComboBox;
  QComboBox *m_dmxDevicesComboBox;
  QPushButton *m_dmxConnect;

  QDmxManager *m_dmxManager;
  QDmxDriver *m_dmxDriver;

};

#endif // DMXMANAGERWIDGET_H
