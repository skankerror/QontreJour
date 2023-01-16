#ifndef DMXSEQUENCE_H
#define DMXSEQUENCE_H

#include <QObject>

class DmxSequence : public QObject
{
  Q_OBJECT
public:
  explicit DmxSequence(QObject *parent = nullptr);

signals:

};

#endif // DMXSEQUENCE_H
