#ifndef QDMXGENERICNETWORKDRIVER_H
#define QDMXGENERICNETWORKDRIVER_H

#include <QNetworkInterface>
#include <QNetworkAddressEntry>

#include "qdmxdriver.h"

class QDmxGenericNetworkDevice;
class QDmxGenericNetworkDriverPrivate;
class Q_DECL_EXPORT QDmxGenericNetworkDriver : public QDmxDriver
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxGenericNetworkDriver)

public:
    QDmxGenericNetworkDriver(QDmxManager* parent = nullptr);
    ~QDmxGenericNetworkDriver() override = default;

    QList<QDmxDevice*> availableDevices() const override;

protected:
    bool loadHook() override;
    bool unloadHook() override;

protected:
    QDmxGenericNetworkDriver(QDmxGenericNetworkDriverPrivate& d, QDmxManager* parent);

    virtual QDmxGenericNetworkDevice* createDevice(const QNetworkInterface& iface,
                                                   const QNetworkAddressEntry& entry) = 0;
};

#endif // QDMXGENERICNETWORKDRIVER_H
