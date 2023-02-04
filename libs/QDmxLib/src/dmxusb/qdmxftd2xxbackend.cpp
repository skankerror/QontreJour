#include <qdmxlib/private/qdmxftd2xxbackend_p.h>
#include <qdmxlib/private/qdmxusbdevice_p.h>
#include <qdmxlib/private/qdmxdriver.h>
#include <qdmxlib/private/qdmxusbglobal.h>

#include <ftd2xx.h>

QDmxFTD2XXBackend::QDmxFTD2XXBackend(QDmxUsbDevicePrivate* device) :
    QDmxUsbBackend(device)
{

}

QDmxFTD2XXBackend::~QDmxFTD2XXBackend()
{

}

bool QDmxFTD2XXBackend::pollDevices(QList<QDmxUsbDevice*>& devices, QDmxUsbDriver* parent)
{
    bool shouldEmit = false;
    QList<QDmxUsbDevice*> tmp;

    DWORD count = 0;

    if(FT_CreateDeviceInfoList(&count) != FT_OK || count < 0)
    {
        qWarning() << "[ftd2xx] Error create device lise";
        return false;
    }

    FT_DEVICE_LIST_INFO_NODE* devInfo = new FT_DEVICE_LIST_INFO_NODE[count];
    QDmxFinally f([devInfo](){ delete[] devInfo; });

    if(FT_GetDeviceInfoList(devInfo, &count) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to get device info list";
        return false;
    }

    for(DWORD i = 0; i < count; i++)
    {
        QString vendor;
        QString name;
        QString serial;
        quint16 vid;
        quint16 pid;

        auto s = getInterfaceInfo(i, vendor, name, serial, vid, pid);
        if(s != FT_OK || name.isEmpty() || serial.isEmpty())
        {
            name = QString(devInfo[i].Description);
            serial = QString(devInfo[i].SerialNumber);
        }

        shouldEmit |= dealWith(devices,
                               tmp,
                               name,
                               serial,
                               vendor,
                               vid,
                               pid,
                               i,
                               QDmxUsbDevice::FTD2XX,
                               parent);
    }

    shouldEmit |= clear(QDmxUsbDevice::FTD2XX, devices, tmp);

    return shouldEmit;
}

QByteArray QDmxFTD2XXBackend::readLabel(quint8 label, int& code)
{
    ft_handle handle = nullptr;

    if(FT_Open(_d->_id, &handle) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to open device";
        return {};
    }

    QDmxFinally f([handle](){ FT_Close(handle); });

    if(FT_ResetDevice(handle) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to reset device";
        return {};
    }

    if(FT_SetBaudRate(handle, 250000u) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to set baud rate";
        return {};
    }

    if(FT_SetDataCharacteristics(handle, FT_BITS_8, FT_STOP_BITS_2, FT_PARITY_NONE) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to set bus characteristics";
        return {};
    }

    if(FT_SetFlowControl(handle, 0, 0, 0) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to set flow control";
        return {};
    }

    QByteArray request;
    request.append(dmxusb_details::enttec_pro_start_of_msg);
    request.append(label);
    request.append(dmxusb_details::enttec_pro_dmx_zero); // data length LSB
    request.append(dmxusb_details::enttec_pro_dmx_zero); // data length MSB
    request.append(dmxusb_details::enttec_pro_end_of_msg);

    DWORD written = 0;
    if (FT_Write(handle, (char*) request.data(), request.size(), &written) != FT_OK)
    {
        qWarning() << "[ftd2xx] Cannot write to device";
        return {};
    }

    if (written == 0)
    {
        qWarning() << "[ftd2xx] Cannot write to device";
        return {};
    }

    QByteArray array(40, 0x00);
    DWORD read = 0;

    FT_SetTimeouts(handle, 500, 0);
    FT_Read(handle, array.data(), array.size(), &read);

    if ((quint8)array[0] != dmxusb_details::enttec_pro_start_of_msg)
    {
        qWarning() << "[ftd2xx] Reply message wrong start code: " << QString::number(array[0], 16);
        return {};
    }

    code = (array[5] << 8) | array[4];
    array.remove(0, 6); // 4 bytes of Enttec protocol + 2 of ESTA ID
    array.replace(dmxusb_details::enttec_pro_end_of_msg, '\0'); // replace Enttec termination with string termination

    return array;
}

bool QDmxFTD2XXBackend::open()
{
    if(isOpen())
        return true;

    if(FT_Open(_d->_id, &_handle) != FT_OK)
    {
        qWarning() << "[ftd2xx] Error opening" << _d->_name;
        return false;
    }

    if(FT_GetLatencyTimer(_handle, &_latency) != FT_OK)
    {
        qWarning() << "[ftd2xx] Error getting latency for" << _d->_name;
        _latency = 16;
    }

    qDebug() << "[ftd2xx]" << _d->_serial << "Default latency is" << _latency;
    return true;
}

bool QDmxFTD2XXBackend::openPID(int pid)
{
    Q_UNUSED(pid);
    return open();
}

bool QDmxFTD2XXBackend::close()
{
    auto s = FT_Close(_handle);
    _handle = nullptr;

    return s == FT_OK;
}

bool QDmxFTD2XXBackend::isOpen()
{
    return _handle;
}

bool QDmxFTD2XXBackend::reset()
{
    return FT_ResetDevice(_handle) == FT_OK;
}

bool QDmxFTD2XXBackend::setLineProperty()
{
    return FT_SetDataCharacteristics(_handle, FT_BITS_8, FT_STOP_BITS_2, FT_PARITY_NONE) == FT_OK;
}

bool QDmxFTD2XXBackend::setBaudRate()
{
    return FT_SetBaudRate(_handle, 250000u) == FT_OK;
}

bool QDmxFTD2XXBackend::setFlowControl()
{
    return FT_SetFlowControl(_handle, 0, 0, 0) == FT_OK;
}

bool QDmxFTD2XXBackend::setLowLatency(bool lowLatency)
{
    quint8 latency = lowLatency ? 1 : _latency;
    return FT_SetLatencyTimer(_handle, latency);
}

bool QDmxFTD2XXBackend::clearRts()
{
    return FT_ClrRts(_handle) == FT_OK;
}

bool QDmxFTD2XXBackend::purgeBuffers()
{
    return FT_Purge(_handle, FT_PURGE_RX | FT_PURGE_TX) == FT_OK;
}

bool QDmxFTD2XXBackend::setBreak(bool on)
{
    auto s = on ? FT_SetBreakOn(_handle)
                : FT_SetBreakOff(_handle);
    return s == FT_OK;
}

bool QDmxFTD2XXBackend::write(const QByteArray& data)
{
    DWORD written = 0;
    return FT_Write(_handle, (char*)data.data(), data.size(), &written) == FT_OK;
}

QByteArray QDmxFTD2XXBackend::read(int size)
{
    QByteArray data(size, 0);
    DWORD RxBytes = 0;
    DWORD TxBytes = 0;
    DWORD event = 0;

    if(FT_GetStatus(_handle, &RxBytes, &TxBytes, &event) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to get bus status for" << _d->_name;
        return {};
    }

    if(RxBytes < (DWORD)size)
        return {};

    DWORD read = 0;
    if(FT_Read(_handle, data.data(), data.size(), &read) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to read" << _d->_name;
        return {};
    }

    return data;
}

quint8 QDmxFTD2XXBackend::readByte(bool* ok)
{
    DWORD RxBytes = 0;
    DWORD TxBytes = 0;
    DWORD event = 0;

    if(FT_GetStatus(_handle, &RxBytes, &TxBytes, &event) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to get bus status for" << _d->_name;
        return {};
    }

    quint8 byte = 0;
    DWORD read = 0;

    if(FT_Read(_handle, &byte, 1, &read) != FT_OK)
    {
        qWarning() << "[ftd2xx] Unable to read" << _d->_name;
        return {};
    }

    if(ok)
        *ok = read == 1;

    return byte;
}

quint32 QDmxFTD2XXBackend::getInterfaceInfo(quint32 devId, QString& vendor, QString& descr,
                                            QString& serial, quint16& vid, quint16& pid)
{
    char cVendor[256];
    char cVendorId[256];
    char cDescription[256];
    char cSerial[256];

    ft_handle handle;

    FT_STATUS status = FT_Open(devId, &handle);
    if (status != FT_OK)
        return status;

    FT_PROGRAM_DATA pData;
    pData.Signature1 = 0;
    pData.Signature2 = 0xFFFFFFFF;
    pData.Version = 0x00000005;
    pData.Manufacturer = cVendor;
    pData.ManufacturerId = cVendorId;
    pData.Description = cDescription;
    pData.SerialNumber = cSerial;
    status = FT_EE_Read(handle, &pData);
    if (status == FT_OK)
    {
        vid = pData.VendorId;
        pid = pData.ProductId;

        if (pData.ProductId == dmxusb_details::dmx4all_pid)
            vendor = "DMX4ALL";
        else
            vendor = QString(cVendor);
        descr = QString(cDescription);
        serial = QString(cSerial);
    }

    FT_Close(handle);

    return status;
}
