#ifndef QDMXGENERICNETWORKDEVICE_P_H
#define QDMXGENERICNETWORKDEVICE_P_H

#include "qdmxdevice_p.h"
#include "qdmxgenericnetworkdevice.h"

class QDmxGenericNetworkDevicePrivate : public QDmxDevicePrivate
{
    Q_DECLARE_PUBLIC(QDmxGenericNetworkDevice);

public:
    QDmxGenericNetworkDevicePrivate(const QNetworkInterface& interface,
                                    const QNetworkAddressEntry& entry,
                                    QDmxDriver* driver);

    QNetworkInterface _interface;
    QNetworkAddressEntry _entry;
    QHostAddress _address;
    QHostAddress _broadcast;
    QString _mac;
};

#endif // QDMXGENERICNETWORKDEVICE_P_H
