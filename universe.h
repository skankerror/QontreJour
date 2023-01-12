#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <QObject>

class Universe : public QObject
{

  Q_OBJECT

public:

  explicit Universe(QObject *parent = nullptr);

signals:

};

#endif // UNIVERSE_H
