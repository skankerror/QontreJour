#ifndef QDMXUSBDEVICE_P_H
#define QDMXUSBDEVICE_P_H

#include "qdmxdevice_p.h"
#include "qdmxusbdevice.h"

class QDmxUsbInterface;
class QDmxUsbDevicePrivate : public QDmxDevicePrivate
{
    Q_DECLARE_PUBLIC(QDmxUsbDevice)

public:
    QDmxUsbDevicePrivate(const QString& name,
                         const QString& serial,
                         const QString& vendor,
                         quint16 vid,
                         quint16 pid,
                         QDmxDriver* parent) :
        QDmxDevicePrivate(parent),
        _name(name),
        _officialName(name),
        _serial(serial),
        _vendor(vendor),
        _vendorId(vid),
        _productId(pid)
    {
    }

    void init();

    QString _name;
    QString _officialName;
    QDmxUsbDevice::Type _type;
    QString _serial;
    QString _vendor;
    quint16 _vendorId;
    quint16 _productId;

    QDmxUsbInterface* _iface = nullptr;
};

namespace dmxusb_details
{
bool checkIds(quint16 vid, quint16 pid);

template<class T, class... Args>
bool dealWith(QList<QDmxUsbDevice*>& devices,
              QList<QDmxUsbDevice*>& tmp,
              const QString& name,
              const QString& serial,
              const QString& vendor,
              quint16 vid,
              quint16 pid,
              Args... args)
{
    auto find_dev = [&](QDmxUsbDevice* _d)
    {
        // Not checking name because it might appear different between libs
        return _d->vendorId() == vid &&
               _d->productId() == pid &&
               _d->vendor() == vendor &&
               _d->serialNumber() == serial;
    };

    auto it = std::find_if(devices.begin(), devices.end(), find_dev);
    if(it != devices.end())
    {
        // We already have this one
        tmp << *it;
        return false;
    }
    else
    {
        // Not found in the existing devices
        auto dev = new T(name, serial, vendor, vid, pid, args...);
        qDebug() << "[dmxusb] New device detected:"
                 << dev->name()
                 << dev->serialNumber()
                 << QString::number(dev->vendorId(), 16)
                 << QString::number(dev->productId(), 16);

        tmp << dev;
        devices << dev;
        return true;
    }
}

template<class T>
bool clear(QList<QDmxUsbDevice*>& devices, QList<QDmxUsbDevice*>& tmp)
{
    bool shouldEmit = false;

    for(auto it = devices.begin(); it != devices.end();)
    {
        auto dev = dynamic_cast<T*>(*it);
        if(!dev)
        {
            ++it;
            continue;
        }

        if(tmp.contains(dev))
        {
            ++it;
            continue;
        }

        shouldEmit = true;

        auto m = dev->manager();
        if(m->isPatched(dev))
            m->unpatch(dev);

        devices.erase(it);
        dev->deleteLater();
    }

    return shouldEmit;
}
}

#endif // QDMXUSBDEVICE_P_H
