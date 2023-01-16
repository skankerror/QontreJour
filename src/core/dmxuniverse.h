#ifndef DMXUNIVERSE_H
#define DMXUNIVERSE_H

#include <QObject>

class DmxUniverse : public QObject
{
  Q_OBJECT
public:
  explicit DmxUniverse(QObject *parent = nullptr);

signals:

};

#endif // DMXUNIVERSE_H
