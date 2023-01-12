#include <qdmxlib/private/qe131driver.h>
#include <qdmxlib/private/qe131device.h>
#include <qdmxlib/private/qdmxgenericnetworkdriver_p.h>

class QE131DriverPrivate : public QDmxGenericNetworkDriverPrivate
{
    Q_DECLARE_PUBLIC(QE131Driver)

public:
    QE131DriverPrivate(QDmxManager* manager) :
        QDmxGenericNetworkDriverPrivate(manager)
    {}

    bool loadConfigHook(const QJsonObject& config) override;
    bool saveConfigHook(QJsonObject& config) override;

    QString _sourceName;
};

bool QE131DriverPrivate::loadConfigHook(const QJsonObject& config)
{
    _sourceName = config["source_name"].toString("QDmxLib");
    return true;
}

bool QE131DriverPrivate::saveConfigHook(QJsonObject& config)
{
    config["source_name"] = _sourceName;
    return true;
}

QE131Driver::QE131Driver(QDmxManager* parent) :
    QDmxGenericNetworkDriver(*new QE131DriverPrivate(parent), parent)
{

}

QString QE131Driver::sourceName() const
{
    return d_func()->_sourceName;
}

void QE131Driver::setSourceName(const QString& n)
{
    d_func()->_sourceName = n;
}

QDmxGenericNetworkDevice* QE131Driver::createDevice(const QNetworkInterface& iface,
                                                    const QNetworkAddressEntry& entry)
{
    return new QE131Device(iface, entry, this);
}
