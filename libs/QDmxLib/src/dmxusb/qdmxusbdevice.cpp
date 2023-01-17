#include <qdmxlib/private/qdmxusbdevice.h>
#include <qdmxlib/private/qdmxusbdevice_p.h>
#include <qdmxlib/private/qdmxusbdriver.h>
#include <qdmxlib/private/qdmxusbglobal.h>

#include <qdmxlib/private/qopendmx.h>
#include <qdmxlib/private/qenttecpro.h>
#include <qdmxlib/private/qeuroliteusbdmxpro.h>
#include <qdmxlib/private/qnanodmx.h>
#include <qdmxlib/private/qvinceusbdmx.h>

void QDmxUsbDevicePrivate::init()
{
    Q_Q(QDmxUsbDevice);

    if (_officialName.contains("PRO MK2"))
    {
        _type = QDmxUsbDevice::ProMk2;
        _outputCount = 2;
        _inputCount = 1;
    }
    else if (_officialName.contains("DMX USB PRO") || _officialName.contains("ULTRADMX"))
    {
        /** Check if the device responds to label 77 and 78, so it might be a DMXking adapter */
        int ESTAID = 0;
        int DEVID = 0;
//        QString manName = _backend->readLabel(dmxusb_details::dmxking_usb_device_manufacturer, ESTAID);
//        qDebug() << "--------> Device Manufacturer: " << manName;
//        QString devName = _backend->readLabel(dmxusb_details::dmxking_usb_device_name, DEVID);
        QString devName = "ProRXTX";
//        qDebug() << "--------> Device Name: " << devName;
//        qDebug() << "--------> ESTA Code: " << QString::number(ESTAID, 16) << ", Device ID: " << QString::number(DEVID, 16);
        if (ESTAID == dmxusb_details::dmxking_esta_id)
        {
            if (DEVID == dmxusb_details::ultradmx_pro_dev_id)
            {
                _type = QDmxUsbDevice::UltraPro;
                _outputCount = 2;
                _inputCount = 0;
                _name = devName;
                _iface = new QEnttecPro(q);
            }
            else
            {
                _type = QDmxUsbDevice::ProRXTX;
                _outputCount = 1;
                _inputCount = 0;
                _name = devName;
                _iface = new QEnttecPro(q);
            }
        }
        else
        {
            _type = QDmxUsbDevice::ProRXTX;
            _outputCount = 1;
//            _inputCount = 1;
            _inputCount = 0;
            _name = devName;
            _iface = new QEnttecPro(q);
        }
    }
    else if (_officialName.contains("DMXIS"))
    {
        _type = QDmxUsbDevice::ProRXTX;
        _outputCount = 1;
        _inputCount = 0;
        _iface = new QEnttecPro(q);
    }
    else if (_officialName.contains("USB-DMX512 CONVERTER"))
    {
        _type = QDmxUsbDevice::VinceTX;
        _outputCount = 1;
        _inputCount = 0;
        _iface = new QVinceUsbDmx(q);
    }
    else if (_vendorId == dmxusb_details::ftdi_vid &&
             _productId == dmxusb_details::dmx4all_pid)
    {
        _type = QDmxUsbDevice::DMX4ALL;
        _outputCount = 1;
        _inputCount = 0;
        _iface = new QNanoDmx(q);
    }
#if defined(Q_WS_X11) || defined(Q_OS_LINUX) || defined(Q_OS_OSX)
    else if (_vendorId == dmxusb_details::atmel_vid &&
             _productId == dmxusb_details::nanodmx_pid)
    {
        _type = QDmxUsbDevice::DMX4ALL;
        _outputCount = 1;
        _inputCount = 0;
        _iface = new QNanoDmx(q);
    }
    else if (_vendorId == dmxusb_details::microchip_vid &&
             _productId == dmxusb_details::eurolite_pid)
    {
        _type = QDmxUsbDevice::Eurolite;
        _outputCount = 1;
        _inputCount = 0;
        _iface = new QEuroliteUsbDmxPro(q);
    }
#endif
    else
    {
        _type = QDmxUsbDevice::OpenTX;
        _outputCount = 1;
        _inputCount = 0;
        _name = "OpenDMX";
        _iface = new QOpenDmx(q);
    }
}

QDmxUsbDevice::QDmxUsbDevice(const QString& name,
                             const QString& serial,
                             const QString& vendor,
                             quint16 vid,
                             quint16 pid,
                             QDmxUsbDevice::Backend backend,
                             QDmxUsbDriver* parent) :
    QDmxDevice(*new QDmxUsbDevicePrivate(name, serial, vendor, vid, pid, backend, parent), parent)
{
    d_func()->init();
}

QDmxUsbDevice::~QDmxUsbDevice()
{
    Q_D(QDmxUsbDevice);

    if(d->_iface->isOpen())
        d->_iface->close();

    delete d->_iface;
}

QDmxUsbDevice::Backend QDmxUsbDevice::backend() const
{
    return d_func()->_backend->backend();
}

QString QDmxUsbDevice::name() const
{
    return d_func()->_name;
}

QString QDmxUsbDevice::officialName() const
{
    return d_func()->_officialName;
}

QDmxUsbDevice::Type QDmxUsbDevice::type() const
{
    return d_func()->_type;
}

QString QDmxUsbDevice::serialNumber() const
{
    return d_func()->_serial;
}

QString QDmxUsbDevice::vendor() const
{
    return d_func()->_vendor;
}

quint16 QDmxUsbDevice::vendorId() const
{
    return d_func()->_vendorId;
}

quint16 QDmxUsbDevice::productId() const
{
    return d_func()->_productId;
}

QDmxUsbBackend* QDmxUsbDevice::privateBackend() const
{
    return d_func()->_backend;
}

void QDmxUsbDevice::setData(quint8 port, quint16 channel, quint8 data)
{
    QDmxDevice::setData(port, channel, data);
    d_func()->_iface->newDataCallback();
}

void QDmxUsbDevice::setData(quint8 port, quint16 channel, const QByteArray& data)
{
    QDmxDevice::setData(port, channel, data);
    d_func()->_iface->newDataCallback();
}

void QDmxUsbDevice::setData(quint8 port, const QByteArray& data)
{
    QDmxDevice::setData(port, data);
    d_func()->_iface->newDataCallback();
}

QDmxUsbDevice::QDmxUsbDevice(QDmxUsbDevicePrivate& d, QDmxUsbDriver* parent) :
    QDmxDevice(d, parent)
{
    d.init();
}

bool QDmxUsbDevice::startHook()
{
    if(!d_func()->_iface->open())
        return false;

    return QDmxDevice::stopHook();
}

bool QDmxUsbDevice::stopHook()
{
    if(!d_func()->_iface->close())
        return false;

    return QDmxDevice::stopHook();
}

void QDmxUsbDevice::updateInput(quint8 port, const QByteArray& data)
{
    Q_D(QDmxUsbDevice);
    d->updateBuffer(d->_inputData, port, data);
    emit inputDataChanged(port, data);
}
