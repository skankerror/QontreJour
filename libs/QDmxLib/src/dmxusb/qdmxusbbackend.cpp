#include <qdmxlib/private/qdmxusbbackend_p.h>
#include <qdmxlib/private/qdmxusbdevice.h>
#include <qdmxlib/private/qdmxusbglobal.h>
#include <qdmxlib/private/qdmxmanager.h>

QDmxUsbBackend::QDmxUsbBackend(QDmxUsbDevicePrivate* device) :
    _d(device)
{

}

QDmxUsbBackend::~QDmxUsbBackend()
{

}

bool QDmxUsbBackend::checkIds(quint16 vid, quint16 pid)
{
    if(vid != dmxusb_details::ftdi_vid &&
       vid != dmxusb_details::atmel_vid &&
       vid != dmxusb_details::microchip_vid)
        return false;

    if(pid != dmxusb_details::ftdi_pid &&
       pid != dmxusb_details::dmx4all_pid &&
       pid != dmxusb_details::nanodmx_pid &&
       pid != dmxusb_details::eurolite_pid &&
       pid != dmxusb_details::electrotas_pid)
        return false;

    return true;
}


bool QDmxUsbBackend::dealWith(QList<QDmxUsbDevice*>& devices,
                              QList<QDmxUsbDevice*>& tmp,
                              const QString& name,
                              const QString& serial,
                              const QString& vendor,
                              quint16 vid,
                              quint16 pid,
                              quint32 id,
                              QDmxUsbDevice::Backend backend,
                              QDmxUsbDriver* parent)
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
        auto dev = new QDmxUsbDevice(name, serial, vendor, vid, pid, id, backend, parent);
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

bool QDmxUsbBackend::clear(QDmxUsbDevice::Backend backend,
                           QList<QDmxUsbDevice*>& devices,
                           QList<QDmxUsbDevice*>& tmp)
{
    bool shouldEmit = false;

    for(auto it = devices.cbegin(); it != devices.cend();)
    {
        auto dev = *it;
        if(dev->backend() != backend)
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
