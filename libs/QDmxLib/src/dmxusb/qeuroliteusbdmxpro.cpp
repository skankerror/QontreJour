#include <qdmxlib/private/qeuroliteusbdmxpro.h>
#include <qdmxlib/private/qdmxusbdevice.h>
#include <qdmxlib/private/qdmxusbglobal.h>

#include <QElapsedTimer>

QEuroliteUsbDmxPro::QEuroliteUsbDmxPro(QDmxUsbDevice* device) :
    QDmxUsbThread(device),
    QDmxUsbInterface(device)
{

}

bool QEuroliteUsbDmxPro::open()
{
    if(isOpen())
        return true;

    if(!init())
        return false;

    start();
    return true;
}

bool QEuroliteUsbDmxPro::close()
{
    if(!isOpen())
        return true;

    gracefullyStop();

    return _device->close();
}

bool QEuroliteUsbDmxPro::isOpen()
{
    return isRunning();
}

void QEuroliteUsbDmxPro::newDataCallback()
{
    _newData.unlock();
}

void QEuroliteUsbDmxPro::run()
{
    QElapsedTimer time;
    QByteArray data;

    while(!isInterruptionRequested())
    {
        time.restart();

        if(!_newData.tryLock(0))
        {
            QThread::yieldCurrentThread();
            continue;
        }

        auto dmx = _device->readOutputData(0);

        data.clear();
        data.append(dmxusb_details::eurolite_usb_dmx_pro_start_of_msg); // Start byte
        data.append(dmxusb_details::eurolite_usb_dmx_pro_send_dmx_rq); // Send request
        data.append((dmx.size() + 1) & 0xff); // Data length LSB
        data.append(((dmx.size() + 1) >> 8) & 0xff); // Data length MSB
        data.append(dmxusb_details::eurolite_usb_dmx_pro_dmx_zero); // DMX start code (Which constitutes the + 1 below)
        data.append(dmx);
        data.append(dmxusb_details::eurolite_usb_dmx_pro_end_of_msg); // Stop byte

        _device->write(data);

        frameSleep(time.elapsed());
    }
}
