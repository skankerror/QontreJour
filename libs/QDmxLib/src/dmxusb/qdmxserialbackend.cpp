#include <qdmxlib/private/qdmxserialbackend_p.h>
#include <qdmxlib/private/qdmxusbdevice_p.h>
#include <qdmxlib/private/qdmxusbglobal.h>

QDmxSerialBackend::QDmxSerialBackend(QDmxUsbDevicePrivate* device) :
    QDmxUsbBackend(device)
{

}

QDmxSerialBackend::~QDmxSerialBackend()
{

}

bool QDmxSerialBackend::pollDevices(QList<QDmxUsbDevice*>& devices, QDmxUsbDriver* parent)
{
    bool shouldEmit = false;
    QList<QDmxUsbDevice*> tmp;

    for(const auto& info : QSerialPortInfo::availablePorts())
    {
        if(!checkIds(info.vendorIdentifier(), info.productIdentifier()))
            continue;

        bool result = dealWith(devices,
                               tmp,
                               info.portName(),
                               info.serialNumber(),
                               info.manufacturer(),
                               info.vendorIdentifier(),
                               info.productIdentifier(),
                               QDmxUsbDevice::Serial,
                               parent);

        if(result)
            dynamic_cast<QDmxSerialBackend*>(tmp.last()->d_func()->_backend)->setInfo(info);

        shouldEmit |= result;
    }

    shouldEmit |= clear(QDmxUsbDevice::Serial, devices, tmp);

    return shouldEmit;
}

void QDmxSerialBackend::setInfo(const QSerialPortInfo& info)
{
    _info = info;
}

QByteArray QDmxSerialBackend::readLabel(quint8 label, int& code)
{
    QSerialPort serial;
    serial.setPort(_info);

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
        qWarning() << "[usbserial]" << _d->_name << "Cannot write data to device";
        return {};
    }
    serial.waitForBytesWritten(20);

    QByteArray array;
    // wait 100ms maximum for the device to respond
    serial.waitForReadyRead(100);

    array = serial.read(40);

    if ((quint8)array[0] != dmxusb_details::enttec_pro_start_of_msg)
    {
        qWarning() << "[usbserial]" << _d->_name << "Reply message wrong start code: " << QString::number(array[0], 16);
        return {};
    }

    code = (array[5] << 8) | array[4];
    array.remove(0, 6); // 4 bytes of Enttec protocol + 2 of ESTA ID
    array.replace(dmxusb_details::enttec_pro_end_of_msg, '\0'); // replace Enttec termination with string termination

    serial.close();

    return array;
}

bool QDmxSerialBackend::open()
{
    if(isOpen())
        return true;

    _serial = new QSerialPort(_info);

    if(!_serial)
    {
        qWarning() << "[usbserial]" << _d->_name << "Cannot create serial port";
        return false;
    }

    if(!_serial->open(QIODevice::ReadWrite))
    {
        _serial->deleteLater();
        _serial = nullptr;
        qWarning() << "[usbserial]" << _d->_name << "Cannot open serial port";
        return false;
    }

    _serial->setReadBufferSize(1024);
    return true;
}

bool QDmxSerialBackend::openPID(int pid)
{
    Q_UNUSED(pid);
    return open();
}

bool QDmxSerialBackend::close()
{
    if(_serial)
    {
        _serial->close();
        _serial->deleteLater();
        _serial = nullptr;
    }

    return true;
}

bool QDmxSerialBackend::isOpen()
{
    return _serial && _serial->isOpen();
}

bool QDmxSerialBackend::reset()
{
    if(!_serial->clear())
    {
        qWarning() << "[usbserial]" << _d->_name << _serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialBackend::setLineProperty()
{
    if(!_serial->setDataBits(QSerialPort::Data8))
    {
        qWarning() << "[usbserial]" << _d->_name << _serial->errorString();
        return false;
    }

    if(!_serial->setStopBits(QSerialPort::TwoStop))
    {
        qWarning() << "[usbserial]" << _d->_name << _serial->errorString();
        return false;
    }

    if(!_serial->setParity(QSerialPort::NoParity))
    {
        qWarning() << "[usbserial]" << _d->_name << _serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialBackend::setBaudRate()
{
    if(!_serial->setBaudRate(250000))
    {
        qWarning() << "[usbserial]" << _d->_name << _serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialBackend::setFlowControl()
{
    if(!_serial->setFlowControl(QSerialPort::NoFlowControl))
    {
        qWarning() << "[usbserial]" << _d->_name << _serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialBackend::setLowLatency(bool lowLatency)
{
    Q_UNUSED(lowLatency);
    return true;
}

bool QDmxSerialBackend::clearRts()
{
    if(!_serial->setRequestToSend(false))
    {
        qWarning() << "[usbserial]" << _d->_name << _serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialBackend::purgeBuffers()
{
    return reset();
}

bool QDmxSerialBackend::setBreak(bool on)
{
    if(!_serial->setBreakEnabled(on))
    {
        qWarning() << "[usbserial]" << _d->_name << _serial->errorString();
        return false;
    }

    return true;
}

bool QDmxSerialBackend::write(const QByteArray& data)
{
    if(_serial->write(data) != data.size())
    {
        qWarning() << "[usbserial]" << _d->_name << _serial->errorString();
        return false;
    }

    return true;
}

QByteArray QDmxSerialBackend::read(int size)
{
    if(_serial->waitForReadyRead(10))
        return _serial->read(size);

    return {};
}

quint8 QDmxSerialBackend::readByte(bool* ok)
{
    if(ok)
        *ok = false;

    if(_serial->waitForReadyRead(10))
    {
        auto data = _serial->read(1);
        if(data.size() > 0)
        {
            if(ok)
                *ok = true;
            return data[0];
        }
    }

    return 0;
}
