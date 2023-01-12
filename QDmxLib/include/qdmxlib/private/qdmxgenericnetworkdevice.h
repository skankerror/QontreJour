#ifndef QDMXGENERICNETWORKDEVICE_H
#define QDMXGENERICNETWORKDEVICE_H

#include <QNetworkInterface>
#include <QNetworkAddressEntry>

#include "qdmxdevice.h"

class QDmxGenericNetworkDriver;
class QDmxGenericNetworkDevicePrivate;
class QDmxGenericNetworkDevice : public QDmxDevice
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxGenericNetworkDevice);

public:
    explicit QDmxGenericNetworkDevice(const QNetworkInterface& interface,
                                      const QNetworkAddressEntry& entry,
                                      QDmxGenericNetworkDriver *parent = nullptr);
    ~QDmxGenericNetworkDevice() override = default;

    QString name() const override;
    QNetworkInterface interface() const;
    QNetworkAddressEntry entry() const;
    QHostAddress broadcast() const;
    QHostAddress address() const;
    QString macAddress() const;

protected:
    QDmxGenericNetworkDevice(QDmxGenericNetworkDevicePrivate& d, QDmxGenericNetworkDriver* parent);
};

#endif // QDMXGENERICNETWORKCONTROLER_H
