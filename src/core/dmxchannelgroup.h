#ifndef DMXCHANNELGROUP_H
#define DMXCHANNELGROUP_H

#include <QObject>
#include <QVector>
#include "dmxchannel.h"

class DmxChannelGroup
    : public QObject
{

  Q_OBJECT

public:

  explicit DmxChannelGroup(QObject *parent = nullptr);

signals:

private:

  QVector<QPair<DmxChannel*, int>> v_p_dmxChannel;
  int m_level;

};

#endif // DMXCHANNELGROUP_H
