#ifndef QE131DEVICE_H
#define QE131DEVICE_H

#include <qdmxlib/private/qdmxgenericnetworkdevice.h>

class QE131DevicePrivate;
class QE131Device : public QDmxGenericNetworkDevice
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QE131Device)

public:
    QE131Device(const QNetworkInterface& interface,
                const QNetworkAddressEntry& entry,
                QDmxGenericNetworkDriver *parent = nullptr);
    ~QE131Device() override = default;

protected slots:
    void setData(quint8 port, quint16 channel, quint8 data) override;
    void setData(quint8 port, quint16 channel, const QByteArray& data) override;
    void setData(quint8 port, const QByteArray& data) override;

    void readDatagram();
    void socketError(QAbstractSocket::SocketError socketError);

protected:
    bool startHook() override;
    bool stopHook() override;
};

#endif // QE131DEVICE_H
