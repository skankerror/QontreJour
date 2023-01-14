#include <qdmxlib/private/qdmxgenericnetworkdevice.h>
#include <qdmxlib/private/qdmxgenericnetworkdriver.h>
#include <qdmxlib/private/qdmxgenericnetworkdevice_p.h>

QDmxGenericNetworkDevicePrivate::QDmxGenericNetworkDevicePrivate(const QNetworkInterface& interface,
                                                                 const QNetworkAddressEntry& entry,
                                                                 QDmxDriver* driver) :
    QDmxDevicePrivate(driver),
    _interface(interface),
    _entry(entry),
    _address(entry.ip())
{
    if (_address == QHostAddress::LocalHost)
    {
        _broadcast = QHostAddress::LocalHost;
        _mac = "11:22:33:44:55:66";
    }
    else
    {
        _broadcast = _entry.broadcast();
        _mac = interface.hardwareAddress();
    }
}

QDmxGenericNetworkDevice::QDmxGenericNetworkDevice(const QNetworkInterface& interface,
                                                   const QNetworkAddressEntry& entry,
                                                   QDmxGenericNetworkDriver *parent) :
    QDmxDevice(*new QDmxGenericNetworkDevicePrivate(interface, entry, parent), parent)
{
}

QString QDmxGenericNetworkDevice::name() const
{
    return d_func()->_interface.name();
}

QNetworkInterface QDmxGenericNetworkDevice::interface() const
{
    return d_func()->_interface;
}

QNetworkAddressEntry QDmxGenericNetworkDevice::entry() const
{
    return d_func()->_entry;
}

QHostAddress QDmxGenericNetworkDevice::broadcast() const
{
    return d_func()->_broadcast;
}

QHostAddress QDmxGenericNetworkDevice::address() const
{
    return d_func()->_address;
}

QString QDmxGenericNetworkDevice::macAddress() const
{
    return d_func()->_mac;
}

QDmxGenericNetworkDevice::QDmxGenericNetworkDevice(QDmxGenericNetworkDevicePrivate& d,
                                                   QDmxGenericNetworkDriver* parent) :
    QDmxDevice(d, parent)
{

}
