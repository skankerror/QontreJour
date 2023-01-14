#include <qdmxlib/private/qenttecpro.h>
#include <qdmxlib/private/qdmxusbdevice.h>
#include <qdmxlib/private/qdmxusbglobal.h>

#include <QElapsedTimer>

QEnttecProInput::QEnttecProInput(QEnttecPro* parent) :
    QDmxUsbThread(parent->_device)
{

}

void QEnttecProInput::run()
{
    auto enttec = dynamic_cast<QEnttecPro*>(parent());
    auto device = enttec->_device;

    QByteArray data;

    while(!isInterruptionRequested())
    {
        if(readData(device, data))
            emit readReady(data);
        else
            QThread::msleep(10);
    }
}

bool QEnttecProInput::readData(QDmxUsbDevice* device, QByteArray& data)
{
    bool ok = false;
    quint8 byte = 0;

    // Skip bytes until we find the start of the next message
    byte = device->readByte(&ok);
    if(!ok || byte != dmxusb_details::enttec_pro_start_of_msg)
        return false;

    // skip non dmx packets
    byte = device->readByte(&ok);
    if(!ok || byte != dmxusb_details::enttec_pro_recv_dmx_pkt)
        return false;

    quint8 lsb = device->readByte(&ok);
    if(!ok)
        return false;

    quint8 msb = device->readByte(&ok);
    if(!ok)
        return false;

    quint16 dataLength = (msb << 8) + lsb;

    // Check status bytes
    byte = device->readByte(&ok);
    if(!ok)
        return false;

    if (byte & 0x01)
        qWarning() << "[enttecpro] Device receive queue overflowed";
    else if (byte & 0x02)
        qWarning() << "[enttecpro] Device receive overrun occurred";

    // Check DMX startcode
    byte = device->readByte(&ok);
    if(!ok)
        return false;

    if (byte != 0x00)
        qWarning() << "[enttecpro] Non-standard DMX startcode received:" << QString::number(byte, 16);
    dataLength -= 2;

    data.clear();
    data = device->read(dataLength);

    // read end byte
    device->readByte(&ok);

    return ok;
}



QEnttecPro::QEnttecPro(QDmxUsbDevice* device) :
    QDmxUsbThread(device),
    QDmxUsbInterface(device),
    _inputThread(new QEnttecProInput(this))
{

}

bool QEnttecPro::open()
{
    if(isOpen())
        return true;

    if(!init())
        return false;

    // Configuring the lines, Enttec only
    // We go full dmx, no midi here, fuck midi
    if(_device->type() != QDmxUsbDevice::UltraPro)
    {
        QByteArray request;
        request.append(dmxusb_details::enttec_pro_start_of_msg); // DMX start code (Which constitutes the + 1 below)
        request.append(dmxusb_details::enttec_pro_enable_api2); // Enable API2
        request.append(char(0x04)); // data length LSB
        request.append(dmxusb_details::enttec_pro_dmx_zero); // data length MSB
        request.append(char(0xAD)); // Magic number. WTF ??
        request.append(char(0x88)); // Magic number. WFT ??
        request.append(char(0xD0)); // Magic number. WTF ??
        request.append(char(0xC8)); // Magic number. WTF ??
        request.append(dmxusb_details::enttec_pro_end_of_msg); // Stop byte

        /* Write "Set API Key Request" message */
        if(!_device->write(request))
        {
            qWarning() << "[enttecpro]" << _device->name() << "Failed to configure ports";
            return false;
        }

        request.clear();
        request.append(dmxusb_details::enttec_pro_start_of_msg);
        request.append(dmxusb_details::enttec_pro_port_ass_req);
        request.append(char(0x02)); // data length LSB - 2 bytes
        request.append(dmxusb_details::enttec_pro_dmx_zero); // data length MSB
        request.append(char(0x01)); // Port 1 enabled for DMX and RDM
        request.append(char(0x01)); // Port 2 enabled for DMX and RDM
        request.append(dmxusb_details::enttec_pro_end_of_msg); // Stop byte

        /* Write "Set Port Assignment Request" message */
        if(!_device->write(request))
        {
            qWarning() << "[enttecpro]" << _device->name() << "Failed to configure ports";
            return false;
        }
    }

    if(_device->inputCount() == 1)
        _inputThread->start();

    start();
    return true;
}

bool QEnttecPro::close()
{
    if(!isOpen())
        return true;

    gracefullyStop();

    if(_inputThread->isRunning())
        _inputThread->gracefullyStop();

    return _device->close();
}

bool QEnttecPro::isOpen()
{
    return isRunning();
}

void QEnttecPro::newDataCallback()
{
  if (_newData.try_lock())
    _newData.unlock();
}

void QEnttecPro::run()
{
    QElapsedTimer time;

    while(!isInterruptionRequested())
    {
        time.restart();

        if(!_newData.tryLock(0))
        {
            QThread::yieldCurrentThread();
            continue;
        }

        _device->write(payload(0)); // dmx for port 0

        if(_device->outputCount() == 2)
            _device->write(payload(1)); // dmx for port 1

        frameSleep(time.elapsed());
    }
}

QByteArray QEnttecPro::payload(quint8 port)
{
    QByteArray data;

    data.append(dmxusb_details::enttec_pro_start_of_msg);

    if(port == 0)
    {
        if(_device->type() == QDmxUsbDevice::UltraPro)
            data.append(dmxusb_details::dmxking_send_port1);
        else
            data.append(dmxusb_details::enttec_pro_send_dmx_rq);
    }
    else
    {
        if(_device->type() == QDmxUsbDevice::UltraPro)
            data.append(dmxusb_details::dmxking_send_port2);
        else
            data.append(dmxusb_details::enttec_pro_rdm_discovery_req2);
    }

    auto dmx = _device->readOutputData(port);

    data.append((dmx.size() + 1) & 0xFF); // length lsb
    data.append((dmx.size() + 1) >> 8); // length msb

    data.append(dmxusb_details::enttec_pro_dmx_zero); // start code
    data.append(dmx);

    data.append(dmxusb_details::enttec_pro_end_of_msg);

    return data;
}

void QEnttecPro::inputDataReady(const QByteArray& data)
{
    _device->updateInput(0, data);
}
