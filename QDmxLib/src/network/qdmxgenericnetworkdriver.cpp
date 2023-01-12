#include <qdmxlib/private/qdmxgenericnetworkdriver.h>
#include <qdmxlib/private/qdmxgenericnetworkdevice.h>
#include <qdmxlib/private/qdmxgenericnetworkdriver_p.h>

QDmxGenericNetworkDriver::QDmxGenericNetworkDriver(QDmxManager *parent) :
    QDmxDriver(*new QDmxGenericNetworkDriverPrivate(parent), parent)
{

}

QList<QDmxDevice*> QDmxGenericNetworkDriver::availableDevices() const
{
    Q_D(const QDmxGenericNetworkDriver);
    return d->convert(d->_ifaces);
}

bool QDmxGenericNetworkDriver::loadHook()
{
    Q_D(QDmxGenericNetworkDriver);

    Q_ASSERT(d->_ifaces.isEmpty());

    for(const QNetworkInterface& iface : QNetworkInterface::allInterfaces())
    {
        for(const QNetworkAddressEntry& entry : iface.addressEntries())
        {
            //for each ip address of this computer, if it is a valid address for that protocol
            if(entry.ip().protocol() != QAbstractSocket::IPv6Protocol && !entry.ip().isLoopback())
                d->_ifaces << createDevice(iface, entry);
        }
    }

    return QDmxDriver::loadHook();
}

bool QDmxGenericNetworkDriver::unloadHook()
{
    Q_D(QDmxGenericNetworkDriver);

    for(auto i : d->_ifaces)
        i->deleteLater();
    d->_ifaces.clear();

    return QDmxDriver::unloadHook();
}

QDmxGenericNetworkDriver::QDmxGenericNetworkDriver(QDmxGenericNetworkDriverPrivate& d,
                                                   QDmxManager* parent) :
    QDmxDriver(d, parent)
{
}
