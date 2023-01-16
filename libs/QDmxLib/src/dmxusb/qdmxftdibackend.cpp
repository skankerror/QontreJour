#include <qdmxlib/private/qdmxftdibackend_p.h>
#include <qdmxlib/private/qdmxusbdevice_p.h>
#include <qdmxlib/private/qdmxdriver.h>
#include <qdmxlib/private/qdmxusbglobal.h>

#include <QDebug>

QDmxFTDIBackend::QDmxFTDIBackend(QDmxUsbDevicePrivate* device) :
    QDmxUsbBackend(device)
{
    _context = ftdi_new();
}


QDmxFTDIBackend::~QDmxFTDIBackend()
{
    ftdi_free(_context);
}

bool QDmxFTDIBackend::pollDevices(QList<QDmxUsbDevice*>& devices, QDmxUsbDriver* parent)
{
    bool shouldEmit = false;
    QList<QDmxUsbDevice*> tmp;

    int ret;
    ftdi_context* context = ftdi_new();
    ftdi_device_list* devList;

    ret = ftdi_usb_find_all(context, &devList, 0, 0);
    if(ret < 0)
    {
        qWarning() << "[ftdi] Unable to poll ftdi devices: " << ftdi_get_error_string(context);
        return false;
    }

    char manufacturer[128];
    char description[128];
    char serial[128];

    for(auto curDev = devList; curDev != nullptr; curDev = curDev->next)
    {
        ret = ftdi_usb_get_strings2(context, curDev->dev,
                                    manufacturer, 128,
                                    description, 128,
                                    serial, 128);
        if(ret < 0)
        {
            qWarning() << "[ftdi] Failed to read device information: "
                       << ftdi_get_error_string(context);
            continue;
        }

        libusb_device_descriptor usb_desc;
        libusb_get_device_descriptor(curDev->dev, &usb_desc);

        if(!checkIds(usb_desc.idVendor, usb_desc.idProduct))
            continue;

        shouldEmit |= dealWith(devices,
                               tmp,
                               description,
                               serial,
                               manufacturer,
                               usb_desc.idVendor,
                               usb_desc.idProduct,
                               QDmxUsbDevice::LibFTDI,
                               parent);
    }

    shouldEmit |= clear(QDmxUsbDevice::LibFTDI, devices, tmp);

    ftdi_free(context);

    return shouldEmit;
}

QByteArray QDmxFTDIBackend::readLabel(quint8 label, int& code)
{
    if (ftdi_usb_open_desc(_context, dmxusb_details::ftdi_vid, dmxusb_details::ftdi_pid,
                           _d->_officialName.toLatin1().data(),
                           _d->_serial.toLatin1().data()) < 0)
        return {};

    if (ftdi_usb_reset(_context) < 0)
        return {};

    if (ftdi_set_baudrate(_context, 250000) < 0)
        return {};

    if (ftdi_set_line_property(_context, BITS_8, STOP_BIT_2, NONE) < 0)
        return {};

    if (ftdi_setflowctrl(_context, SIO_DISABLE_FLOW_CTRL) < 0)
        return {};

    QByteArray request;
    request.append(dmxusb_details::enttec_pro_start_of_msg);
    request.append(label);
    request.append(dmxusb_details::enttec_pro_dmx_zero); // data length LSB
    request.append(dmxusb_details::enttec_pro_dmx_zero); // data length MSB
    request.append(dmxusb_details::enttec_pro_end_of_msg);

    if (ftdi_write_data(_context, (uchar*) request.data(), request.size()) < 0)
    {
        qWarning() << "[ftdi] Cannot write data to device";
        return {};
    }

    QByteArray array(40, 0x00);
    usleep(300000); // give some time to the device to respond

    ftdi_read_data(_context, (uchar*)array.data(), array.size());

    if ((quint8)array[0] != dmxusb_details::enttec_pro_start_of_msg)
    {
        qWarning() << "[ftdi] Reply message wrong start code: " << QString::number(array[0], 16);
        return {};
    }

    code = (array[5] << 8) | array[4];
    array.remove(0, 6); // 4 bytes of Enttec protocol + 2 of ESTA ID
    array.replace(dmxusb_details::enttec_pro_end_of_msg, '\0'); // replace Enttec termination with string termination

    ftdi_usb_close(_context);

    return array;
}

bool QDmxFTDIBackend::open()
{
    if(isOpen())
        return true;

    QByteArray sba = _d->_serial.toLatin1();
    const char *ser = nullptr;
    if (sba.isEmpty())
        ser = sba.data();

    if(ftdi_usb_open_desc(_context, _d->_vendorId, _d->_productId,
                          _d->_officialName.toLatin1(), ser) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    if(ftdi_get_latency_timer(_context, &_latency))
    {
        qWarning() << "[ftdi]" << _d->_serial << ftdi_get_error_string(_context) << "while querying latency";
        _latency = 16;
    }

    qDebug() << "[ftdi]" << _d->_serial << "Default latency is" << _latency;
    return true;
}

bool QDmxFTDIBackend::openPID(int pid)
{
    if(isOpen())
        return true;

    if(ftdi_usb_open(_context, dmxusb_details::ftdi_vid, pid) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    if(ftdi_get_latency_timer(_context, &_latency))
    {
        qWarning() << "[ftdi]" << _d->_serial << ftdi_get_error_string(_context) << "while querying latency";
        _latency = 16;
    }

    qDebug() << "[ftdi]" << _d->_name << "Default latency is" << _latency;
    return true;
}

bool QDmxFTDIBackend::close()
{
    if(!isOpen())
        return true;

    if(ftdi_usb_close(_context) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    return true;
}

bool QDmxFTDIBackend::isOpen()
{
    return _context->usb_dev;
}

bool QDmxFTDIBackend::reset()
{
    if(ftdi_usb_reset(_context) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    return true;
}

bool QDmxFTDIBackend::setLineProperty()
{
    if(ftdi_set_line_property(_context, BITS_8, STOP_BIT_2, NONE) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    return true;
}

bool QDmxFTDIBackend::setBaudRate()
{
    if(ftdi_set_baudrate(_context, 250000) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    return true;
}

bool QDmxFTDIBackend::setFlowControl()
{
    if(ftdi_setflowctrl(_context, SIO_DISABLE_FLOW_CTRL) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    return true;
}

bool QDmxFTDIBackend::setLowLatency(bool lowLatency)
{
    quint8 latency;
    if(lowLatency)
        latency = 1;
    else
        latency = _latency;

    if(ftdi_set_latency_timer(_context, latency) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    qDebug() << "[ftdi]" << _d->_name << "Latency set to" << latency;
    return true;
}

bool QDmxFTDIBackend::clearRts()
{
    if(ftdi_setrts(_context, 0) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    return true;
}

bool QDmxFTDIBackend::purgeBuffers()
{
    if(ftdi_tcioflush(_context) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    return true;
}

bool QDmxFTDIBackend::setBreak(bool on)
{
    ftdi_break_type type = on ? BREAK_ON : BREAK_OFF;

    if(ftdi_set_line_property2(_context, BITS_8, STOP_BIT_2, NONE, type) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    return true;
}

bool QDmxFTDIBackend::write(const QByteArray& data)
{
    if(ftdi_write_data(_context, (uchar*)data.data(), data.size()) < 0)
    {
        qWarning() << "[ftdi]" << _d->_name << ftdi_get_error_string(_context);
        return false;
    }

    return true;
}

QByteArray QDmxFTDIBackend::read(int size)
{
    QByteArray data(size, 0);
    ftdi_read_data(_context, (uchar*)data.data(), size);

    return data;
}

quint8 QDmxFTDIBackend::readByte(bool* ok)
{
    quint8 byte = 0;
    int read = ftdi_read_data(_context, &byte, 1);

    if(ok)
        *ok = read == 1;

    return byte;
}
