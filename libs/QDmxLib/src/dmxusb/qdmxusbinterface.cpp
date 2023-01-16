#include <qdmxlib/private/qdmxusbinterface.h>
#include <qdmxlib/private/qdmxusbdevice.h>
#include <qdmxlib/private/qdmxusbbackend_p.h>

QDmxUsbInterface::QDmxUsbInterface(QDmxUsbDevice* device) :
    _device(device),
    _backend(device->privateBackend())
{

}

bool QDmxUsbInterface::init()
{
    if(!_backend->open())
    {
        _backend->close();
        return false;
    }

    if(!_backend->clearRts())
    {
        _backend->close();
        return false;
    }

    return true;
}

QDmxUsbThread::QDmxUsbThread(QDmxUsbDevice* device) : QThread(device) {}

void QDmxUsbThread::gracefullyStop()
{
    requestInterruption();
    if(!wait(10000))
    {
        terminate();
        wait(10000);
    }
}

void QDmxUsbThread::frameSleep(qint64 elapsed)
{
    // Make sure we wait at least 1 frame length before pushing new data
    // 25ms is a bit longer than max speed (which is ~22ms)
    int remaining = 25 - elapsed;
    if(remaining > 0)
        QThread::msleep(remaining);
}
