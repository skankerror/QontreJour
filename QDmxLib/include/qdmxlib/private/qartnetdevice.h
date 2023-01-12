#ifndef QARTNETDEVICE_H
#define QARTNETDEVICE_H

#include "qdmxgenericnetworkdevice.h"

class QArtnetDevicePrivate;
class Q_DECL_EXPORT QArtnetDevice : public QDmxGenericNetworkDevice
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QArtnetDevice)

    friend class QArtnetDriver;
    friend class QArtnetDriverPrivate;

public:
    QArtnetDevice(const QNetworkInterface& interface,
                  const QNetworkAddressEntry& entry,
                  QDmxGenericNetworkDriver *parent = nullptr);
    ~QArtnetDevice() override = default;

protected slots:
    void setData(quint8 port, quint16 channel, quint8 data) override;
    void setData(quint8 port, quint16 channel, const QByteArray& data) override;
    void setData(quint8 port, const QByteArray& data) override;

    void readDatagram();
    void socketError(QAbstractSocket::SocketError socketError);

    void sendDmx();
    void sendPollReply();

    void checkSubscriptions();

protected:
    bool startHook() override;
    bool stopHook() override;
};

#endif // QARTNETDEVICE_H
