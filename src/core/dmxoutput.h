#ifndef DMXOUTPUT_H
#define DMXOUTPUT_H

#include <QObject>
//#include <QByteArray>

class DmxOutput
    : public QObject
{

  Q_OBJECT

public:

  explicit DmxOutput(QObject *parent = nullptr);

signals:

private:

  int m_level;
  // TODO : add curves output

};

#endif // DMXOUTPUT_H
