#include <qdmxlib/private/qvinceusbdmx.h>
#include <qdmxlib/private/qdmxusbdevice.h>
#include <qdmxlib/private/qdmxusbglobal.h>

QVinceUsbDmx::QVinceUsbDmx(QDmxUsbDevice* device) :
    QDmxUsbInterface(device)
{

}

bool QVinceUsbDmx::open()
{
    if(isOpen())
        return true;

    if(!_device->open())
        return false;

    if(!_device->clearRts())
        return false;

    if(!_device->write(QByteArray(2, 0x00)))
        return false;

    return sendCommand(dmxusb_details::vince_cmd_start_dmx);
}

bool QVinceUsbDmx::close()
{
    if(!isOpen())
        return true;

    if(sendCommand(dmxusb_details::vince_cmd_stop_dmx))
        return _device->close();

    return false;
}

bool QVinceUsbDmx::isOpen()
{
    return _device->isOpen();
}

void QVinceUsbDmx::newDataCallback()
{
    auto dmx = _device->readOutputData(0);
    if(dmx == _compare)
        return;

    if(!sendCommand(dmxusb_details::vince_cmd_update_dmx, dmx))
    {
        qWarning() << "[vinceusbdmx] will not accept DMX data";
        return;
    }

    if(!checkRely())
    {
        qWarning() << "[vinceusbdmx] doesn't respond properly";
        return;
    }

    _compare = dmx;
}

bool QVinceUsbDmx::sendCommand(quint8 command, const QByteArray& data)
{
    QByteArray msg(2, dmxusb_details::vince_start_of_msg);
    msg.append(command);

    if(data.isEmpty())
        msg.append(QByteArray(2, 0x00));
    else
    {
        msg.append(((data.size() + 2) >> 8) & 0xFF);
        msg.append((data.size() + 2) & 0xFF);
        msg.append(QByteArray(2, 0x00));
        msg.append(data);
    }

    msg.append(dmxusb_details::vince_end_of_msg);

    return _device->write(msg);
}

bool QVinceUsbDmx::checkRely()
{
    bool ok = false;
    quint8 byte = 0;
    quint16 length = 0;
    QByteArray data;

    for(int i = 0; i < 6; i++)
    {
        byte = _device->readByte(&ok);
        if(!ok)
            return false;


        switch(i)
        {
        default:
            break;

        case 3:
        {
            qWarning() << "[vinceusbdmx] Error" << byte << "in readed message";
            return false;
        }

        case 4:
            length = byte << 8;
            break;

        case 5:
            length += byte;
            break;
        }
    }

    if(length > 0)
    {
        for (int i = 0; i < length; i++)
        {
            byte = _device->readByte(&ok);
            if(!ok)
            {
                qWarning() << "[vinceusbdmx] No available byte to read (" << (length - i) << "missing bytes)";
                return false;
            }

            data.append(byte);
        }
    }

    byte = _device->readByte(&ok);
    if(!ok)
        return false;

    if(byte != dmxusb_details::vince_end_of_msg)
    {
        qWarning() << "[vinceusbdmx] Incorrect end of message received:" << byte;
        return false;
    }

    if(data.size() > 0)
    {
        qWarning() << "[vinceusbdmx] doesn't respond properly";
        return false;
    }

    return true;
}
