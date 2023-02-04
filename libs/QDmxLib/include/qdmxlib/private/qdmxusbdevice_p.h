#ifndef QDMXUSBDEVICE_P_H
#define QDMXUSBDEVICE_P_H

#include "qdmxdevice_p.h"
#include "qdmxusbdevice.h"

#ifdef QDMXLIB_HAS_FTDI
#include <qdmxlib/private/qdmxftdibackend_p.h>
#endif

#ifdef QDMXLIB_HAS_FTD2XX
#include <qdmxlib/private/qdmxftd2xxbackend_p.h>
#endif

#ifdef QDMXLIB_HAS_QTSERIAL
#include <qdmxlib/private/qdmxserialbackend_p.h>
#endif

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
                         quint32 id,
                         QDmxUsbDevice::Backend backend,
                         QDmxDriver* parent) :
        QDmxDevicePrivate(parent),
        _name(name),
        _officialName(name),
        _serial(serial),
        _vendor(vendor),
        _vendorId(vid),
        _productId(pid),
        _id(id)
    {
        switch (backend) {
        case QDmxUsbDevice::LibFTDI:
#ifdef QDMXLIB_HAS_FTDI
            _backend.reset(new QDmxFTDIBackend(this));
#endif
            break;
        case QDmxUsbDevice::FTD2XX:
#ifdef QDMXLIB_HAS_FTD2XX
            _backend.reset(new QDmxFTD2XXBackend(this));
#endif
            break;
        case QDmxUsbDevice::Serial:
#ifdef QDMXLIB_HAS_QTSERIAL
            _backend.reset(new QDmxSerialBackend(this));
#endif
            break;
        default:
            break;
        }
    }

    void init();

    QString _name;
    QString _officialName;
    QDmxUsbDevice::Type _type;
    QString _serial;
    QString _vendor;
    quint16 _vendorId;
    quint16 _productId;
    quint32 _id;
    std::unique_ptr<QDmxUsbBackend> _backend;

    QDmxUsbInterface* _iface = nullptr;
};

#endif // QDMXUSBDEVICE_P_H
