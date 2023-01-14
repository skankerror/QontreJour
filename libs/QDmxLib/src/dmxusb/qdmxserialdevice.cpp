#include <qdmxlib/private/qdmxserialdevice.h>
#include <qdmxlib/private/qdmxusbdevice_p.h>
#include <qdmxlib/private/qdmxusbdriver.h>
#include <qdmxlib/private/qdmxusbglobal.h>

#include <QSerialPort>

class QDmxSerialDevicePrivate : public QDmxUsbDevicePrivate
{
    Q_DECLARE_PUBLIC(QDmxSerialDevice)

public:
    QDmxSerialDevicePrivate(const QString& name,
                          const QString& serial,
                          const QString& vendor,
                          quint16 vid,
                          quint16 pid,
                          const QSerialPortInfo& info,
                          QDmxUsbDriver* parent) :
        QDmxUsbDevicePrivate(name, serial, vendor, vid, pid, parent),
        _info(info)
    {
    }

    ~QDmxSerialDevicePrivate() override = default;

    QSerialPortInfo _info;
    QSerialPort* _serial = nullptr;
};

QDmxSerialDevice::QDmxSerialDevice(const QString& name,
                                   const QString& serial,
                                   const QString& vendor,
                                   quint16 vid,
                                   quint16 pid,
                                   const QSerialPortInfo& info,
                                   QDmxUsbDriver* parent) :
    QDmxUsbDevice(*new QDmxSerialDevicePrivate(name, serial, vendor, vid, pid, info, parent), parent)
{
}

bool QDmxSerialDevice::pollDevices(QList<QDmxUsbDevice*>& devices, QDmxUsbDriver* parent)
{
    bool shouldEmit = false;
    QList<QDmxUsbDevice*> tmp;

    for(const auto& info : QSerialPortInfo::availablePorts())
    {
        if(!dmxusb_details::checkIds(info.vendorIdentifier(), info.productIdentifier()))
            continue;

        shouldEmit |= dmxusb_details::dealWith<QDmxSerialDevice>(devices,
                                                                 tmp,
                                                                 info.portName(),
                                                                 info.serialNumber(),
                                                                 info.manufacturer(),
                                                                 info.vendorIdentifier(),
                                                                 info.productIdentifier(),
                                                                 info,
                                                                 parent);
    }

    shouldEmit |= dmxusb_details::clear<QDmxSerialDevice>(devices, tmp);

    return shouldEmit;
}

QByteArray QDmxSerialDevice::readLabel(quint8 label, int& code)
{
    Q_D(QDmxSerialDevice);

    QSerialPort serial;
    serial.setPort(d->_info);

    if (serial.open(QIODevice::ReadWrite) == false)
        return {};

    serial.setReadBufferSize(1024);
    serial.setDataBits(QSerialPort::Data8);
    serial.setStopBits(QSerialPort::TwoStop);
    serial.setParity(QSerialPort::NoParity);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.setBaudRate(250000);

    QByteArray request;
    request.append(dmxusb_details::enttec_pro_start_of_msg);
    request.append(label);
    request.append(dmxusb_details::enttec_pro_dmx_zero); // data length LSB
    request.append(dmxusb_details::enttec_pro_dmx_zero); // data length MSB
    request.append(dmxusb_details::enttec_pro_end_of_msg);

    if (serial.write(request) < 0)
    {
        qWarning() << "[usbserial]" << name() << "Cannot write data to device";
        return {};
    }
    serial.waitForBytesWritten(20);

    QByteArray array;
    // wait 100ms maximum for the device to respond
    serial.waitForReadyRead(100);

    array = serial.read(40);

    if ((quint8)array[0] != dmxusb_details::enttec_pro_start_of_msg)
    {
        qWarning() << "[usbserial]" << name() << "Reply message wrong start code: " << QString::number(array[0], 16);
        return {};
    }

    code = (array[5] << 8) | array[4];
    array.remove(0, 6); // 4 bytes of Enttec protocol + 2 of ESTA ID
    array.replace(dmxusb_details::enttec_pro_end_of_msg, '\0'); // replace Enttec termination with string termination

    serial.close();

    return array;
}

bool QDmxSerialDevice::open()
{
    Q_D(QDmxSerialDevice);

    if(isOpen())
        return true;

    d->_serial = new QSerialPort(d->_info);

    if(!d->_serial)
    {
        qWarning() << "[usbserial]" << name() << "Cannot create serial port";
        return false;
    }

    if(!d->_serial->open(QIODevice::ReadWrite))
    {
        d->_serial->deleteLater();
        d->_serial = nullptr;
        qWarning() << "[usbserial]" << name() << "Cannot open serial port";
        return false;
    }

    d->_serial->setReadBufferSize(1024);
    return true;
}

bool QDmxSerialDevice::openPID(int pid)
{
    Q_UNUSED(pid);
    return open();
}

bool QDmxSerialDevice::close()
{
    Q_D(QDmxSerialDevice);
    if(d->_serial)
    {
        d->_serial->close();
        d->_serial->deleteLater();
        d->_serial = nullptr;
    }

    return true;
}

bool QDmxSerialDevice::isOpen()
{
    Q_D(QDmxSerialDevice);
    return d->_serial && d->_serial->isOpen();
}

bool QDmxSerialDevice::reset()
{
    Q_D(QDmxSerialDevice);
    if(!d->_serial->clear())
    {
        qWarning() << "[usbserial]" << name() << d->_serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialDevice::setLineProperty()
{
    Q_D(QDmxSerialDevice);

    if(!d->_serial->setDataBits(QSerialPort::Data8))
    {
        qWarning() << "[usbserial]" << name() << d->_serial->errorString();
        return false;
    }

    if(!d->_serial->setStopBits(QSerialPort::TwoStop))
    {
        qWarning() << "[usbserial]" << name() << d->_serial->errorString();
        return false;
    }

    if(!d->_serial->setParity(QSerialPort::NoParity))
    {
        qWarning() << "[usbserial]" << name() << d->_serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialDevice::setBaudRate()
{
    Q_D(QDmxSerialDevice);
    if(!d->_serial->setBaudRate(250000))
    {
        qWarning() << "[usbserial]" << name() << d->_serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialDevice::setFlowControl()
{
    Q_D(QDmxSerialDevice);
    if(!d->_serial->setFlowControl(QSerialPort::NoFlowControl))
    {
        qWarning() << "[usbserial]" << name() << d->_serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialDevice::setLowLatency(bool lowLatency)
{
    Q_UNUSED(lowLatency);
    return true;
}

bool QDmxSerialDevice::clearRts()
{
    Q_D(QDmxSerialDevice);
    if(!d->_serial->setRequestToSend(false))
    {
        qWarning() << "[usbserial]" << name() << d->_serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialDevice::purgeBuffers()
{
    return reset();
}

bool QDmxSerialDevice::setBreak(bool on)
{
    Q_D(QDmxSerialDevice);
    if(!d->_serial->setBreakEnabled(on))
    {
        qWarning() << "[usbserial]" << name() << d->_serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialDevice::write(const QByteArray& data)
{
    Q_D(QDmxSerialDevice);
    if(d->_serial->write(data) != data.size())
    {
        qWarning() << "[usbserial]" << name() << d->_serial->errorString();
        return false;
    }

    return true;
}

QByteArray QDmxSerialDevice::read(int size)
{
    Q_D(QDmxSerialDevice);

    if(d->_serial->waitForReadyRead(10))
        return d->_serial->read(size);

    return {};
}

quint8 QDmxSerialDevice::readByte(bool* ok)
{
    Q_D(QDmxSerialDevice);

    if(ok)
        *ok = false;

    if(d->_serial->waitForReadyRead(10))
    {
        auto data = d->_serial->read(1);
        if(data.size() > 0)
        {
            if(ok)
                *ok = true;
            return data[0];
        }
    }

    return 0;
}
