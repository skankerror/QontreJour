#ifndef DMXSCENE_H
#define DMXSCENE_H

#include <QObject>

class DmxScene : public QObject
{
  Q_OBJECT
public:
  explicit DmxScene(QObject *parent = nullptr);

signals:

};

#endif // DMXSCENE_H
