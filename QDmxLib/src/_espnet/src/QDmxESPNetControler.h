#ifndef QDMXESPNetCONTROLER_H
#define QDMXESPNetCONTROLER_H

#include "QDmxESPNetPacketizer.h"
#include <network/QDmxGenericNetworkControler.h>
#include <QUdpSocket>

#define ESPNET_PORT 3333

class QDmxESPNetControler : public QDmxGenericNetworkControler
{
    Q_OBJECT
public:
    explicit QDmxESPNetControler(QNetworkInterface interface, QNetworkAddressEntry entry, QObject *parent = nullptr);
    ~QDmxESPNetControler();

    virtual quint32 getNbreInputPort() const { return 255; }
    quint32 getNbreOutputPort() const { return 255; }

    virtual bool listenToUniverse(quint32 u);
    virtual bool ignoreListenedUniverse(quint32 u);

    virtual bool writeToUniverse(quint32 u);
    virtual bool ignoreWrittenUniverse(quint32 u);

public slots:
    virtual void write(quint32 u, QByteArray data);

private slots:
    void readDatagram();

private:
    void socketNeeded();
    void socketUnneeded();
    bool socketIsOk() const { return _s && _s->state() == QUdpSocket::BoundState; }
    void sendPollReply();

private:
    QUdpSocket* _s;

    QList<quint32> _listenedUniverse;
    QList<quint32> _writtenUniverse;

    QDmxESPNetPacketizer* _packetizer;
};

typedef QSharedPointer<QDmxESPNetControler> QDmxESPNetControler_ptr;

#endif // QDMXESPNetCONTROLER_H
