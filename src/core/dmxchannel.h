#ifndef DMXCHANNEL_H
#define DMXCHANNEL_H

#include <QObject>
#include <QVector>
#include "dmxoutput.h"

class DmxChannel
    : public QObject
{

  Q_OBJECT

public:

  explicit DmxChannel(QObject *parent = nullptr);

signals:

private:

  QVector<DmxOutput*> v_m_dmxOutput;
  int m_level;

};

#endif // DMXCHANNEL_H
