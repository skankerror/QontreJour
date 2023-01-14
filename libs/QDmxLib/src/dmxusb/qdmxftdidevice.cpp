#include <qdmxlib/private/qdmxftdidevice.h>
#include <qdmxlib/private/qdmxusbdevice_p.h>
#include <qdmxlib/private/qdmxusbdriver.h>
#include <qdmxlib/private/qdmxusbglobal.h>

#include <QDebug>
#include <QThread>

#include <unistd.h>
#include <ftdi.h>

#ifdef Q_OS_LINUX
#include <libusb-1.0/libusb.h>
#else
#include <libusb.h>
#endif

class QDmxFtdiDevicePrivate : public QDmxUsbDevicePrivate
{
    Q_DECLARE_PUBLIC(QDmxFtdiDevice)

public:
    QDmxFtdiDevicePrivate(const QString& name,
                          const QString& serial,
                          const QString& vendor,
                          quint16 vid,
                          quint16 pid,
                          QDmxUsbDriver* parent) :
        QDmxUsbDevicePrivate(name, serial, vendor, vid, pid, parent)
    {
        _context = ftdi_new();
    }

    ~QDmxFtdiDevicePrivate() override
    {
        ftdi_free(_context);
    }

    ftdi_context* _context = nullptr;
    quint8 _latency = 16;
};

class QDmxFtdiThread : public QThread
{

};

QDmxFtdiDevice::QDmxFtdiDevice(const QString& name,
                               const QString& serial,
                               const QString& vendor,
                               quint16 vid,
                               quint16 pid,
                               QDmxUsbDriver* parent) :
    QDmxUsbDevice(*new QDmxFtdiDevicePrivate(name, serial, vendor, vid, pid, parent), parent)
{
}

bool QDmxFtdiDevice::pollDevices(QList<QDmxUsbDevice*>& devices, QDmxUsbDriver* parent)
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

        if(!dmxusb_details::checkIds(usb_desc.idVendor, usb_desc.idProduct))
            continue;

        shouldEmit |= dmxusb_details::dealWith<QDmxFtdiDevice>(devices,
                                                               tmp,
                                                               description,
                                                               serial,
                                                               manufacturer,
                                                               usb_desc.idVendor,
                                                               usb_desc.idProduct,
                                                               parent);
    }

    shouldEmit |= dmxusb_details::clear<QDmxFtdiDevice>(devices, tmp);

    ftdi_free(context);

    return shouldEmit;
}

QByteArray QDmxFtdiDevice::readLabel(quint8 label, int& code)
{
    Q_D(QDmxFtdiDevice);

    if (ftdi_usb_open_desc(d->_context, dmxusb_details::ftdi_vid, dmxusb_details::ftdi_pid,
                           d->_officialName.toLatin1().data(),
                           d->_serial.toLatin1().data()) < 0)
        return {};

    if (ftdi_usb_reset(d->_context) < 0)
        return {};

    if (ftdi_set_baudrate(d->_context, 250000) < 0)
        return {};

    if (ftdi_set_line_property(d->_context, BITS_8, STOP_BIT_2, NONE) < 0)
        return {};

    if (ftdi_setflowctrl(d->_context, SIO_DISABLE_FLOW_CTRL) < 0)
        return {};

    QByteArray request;
    request.append(dmxusb_details::enttec_pro_start_of_msg);
    request.append(label);
    request.append(dmxusb_details::enttec_pro_dmx_zero); // data length LSB
    request.append(dmxusb_details::enttec_pro_dmx_zero); // data length MSB
    request.append(dmxusb_details::enttec_pro_end_of_msg);

    if (ftdi_write_data(d->_context, (uchar*) request.data(), request.size()) < 0)
    {
        qWarning() << "[ftdi] Cannot write data to device";
        return {};
    }

    QByteArray array(40, 0x00);
    usleep(300000); // give some time to the device to respond

    ftdi_read_data(d->_context, (uchar*)array.data(), array.size());

    if ((quint8)array[0] != dmxusb_details::enttec_pro_start_of_msg)
    {
        qWarning() << "[ftdi] Reply message wrong start code: " << QString::number(array[0], 16);
        return {};
    }

    code = (array[5] << 8) | array[4];
    array.remove(0, 6); // 4 bytes of Enttec protocol + 2 of ESTA ID
    array.replace(dmxusb_details::enttec_pro_end_of_msg, '\0'); // replace Enttec termination with string termination

    ftdi_usb_close(d->_context);

    return array;
}

bool QDmxFtdiDevice::open()
{
    Q_D(QDmxFtdiDevice);

    if(isOpen())
        return true;

    QByteArray sba = serialNumber().toLatin1();
    const char *ser = nullptr;
    if (sba.isEmpty())
        ser = sba.data();

    if(ftdi_usb_open_desc(d->_context, vendorId(), productId(),
                          d->_officialName.toLatin1(), ser) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    if(ftdi_get_latency_timer(d->_context, &d->_latency))
    {
        qWarning() << "[ftdi]" << serialNumber() << ftdi_get_error_string(d->_context) << "while querying latency";
        d->_latency = 16;
    }

    qDebug() << "[ftdi]" << serialNumber() << "Default latency is" << d->_latency;
    return true;
}

bool QDmxFtdiDevice::openPID(int pid)
{
    Q_D(QDmxFtdiDevice);

    if(isOpen())
        return true;

    if(ftdi_usb_open(d->_context, dmxusb_details::ftdi_vid, pid) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    if(ftdi_get_latency_timer(d->_context, &d->_latency))
    {
        qWarning() << "[ftdi]" << serialNumber() << ftdi_get_error_string(d->_context) << "while querying latency";
        d->_latency = 16;
    }

    qDebug() << "[ftdi]" << name() << "Default latency is" << d->_latency;
    return true;
}

bool QDmxFtdiDevice::close()
{
    Q_D(QDmxFtdiDevice);

    if(!isOpen())
        return true;

    if(ftdi_usb_close(d->_context) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    return true;
}

bool QDmxFtdiDevice::isOpen()
{
    return d_func()->_context->usb_dev;
}

bool QDmxFtdiDevice::reset()
{
     Q_D(QDmxFtdiDevice);

    if(ftdi_usb_reset(d->_context) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    return true;
}

bool QDmxFtdiDevice::setLineProperty()
{
    Q_D(QDmxFtdiDevice);

    if(ftdi_set_line_property(d->_context, BITS_8, STOP_BIT_2, NONE) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    return true;
}

bool QDmxFtdiDevice::setBaudRate()
{
    Q_D(QDmxFtdiDevice);

    if(ftdi_set_baudrate(d->_context, 250000) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    return true;
}

bool QDmxFtdiDevice::setFlowControl()
{
    Q_D(QDmxFtdiDevice);

    if(ftdi_setflowctrl(d->_context, SIO_DISABLE_FLOW_CTRL) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    return true;
}

bool QDmxFtdiDevice::setLowLatency(bool lowLatency)
{
    Q_D(QDmxFtdiDevice);

    quint8 latency;
    if(lowLatency)
        latency = 1;
    else
        latency = d->_latency;

    if(ftdi_set_latency_timer(d->_context, latency) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    qDebug() << "[ftdi]" << name() << "Latency set to" << latency;
    return true;
}

bool QDmxFtdiDevice::clearRts()
{
    Q_D(QDmxFtdiDevice);

    if(ftdi_setrts(d->_context, 0) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    return true;
}

bool QDmxFtdiDevice::purgeBuffers()
{
    Q_D(QDmxFtdiDevice);

    if(ftdi_tcioflush(d->_context) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    return true;
}

bool QDmxFtdiDevice::setBreak(bool on)
{
    Q_D(QDmxFtdiDevice);

    ftdi_break_type type = on ? BREAK_ON : BREAK_OFF;

    if(ftdi_set_line_property2(d->_context, BITS_8, STOP_BIT_2, NONE, type) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    return true;
}

bool QDmxFtdiDevice::write(const QByteArray& data)
{
    Q_D(QDmxFtdiDevice);

    if(ftdi_write_data(d->_context, (uchar*)data.data(), data.size()) < 0)
    {
        qWarning() << "[ftdi]" << name() << ftdi_get_error_string(d->_context);
        return false;
    }

    return true;
}

QByteArray QDmxFtdiDevice::read(int size)
{
    Q_D(QDmxFtdiDevice);

    QByteArray data(size, 0);
    ftdi_read_data(d->_context, (uchar*)data.data(), size);

    return data;
}

quint8 QDmxFtdiDevice::readByte(bool* ok)
{
    Q_D(QDmxFtdiDevice);
    quint8 byte = 0;
    int read = ftdi_read_data(d->_context, &byte, 1);

    if(ok)
        *ok = read == 1;

    return byte;
}
