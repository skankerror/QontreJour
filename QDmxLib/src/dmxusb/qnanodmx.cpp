#include <qdmxlib/private/qnanodmx.h>
#include <qdmxlib/private/qdmxusbdevice.h>
#include <qdmxlib/private/qdmxusbglobal.h>

#include <QElapsedTimer>

QNanoDmx::QNanoDmx(QDmxUsbDevice* device) :
    QDmxUsbThread(device),
    QDmxUsbInterface(device)
{

}

bool QNanoDmx::open()
{
    if(isOpen())
        return true;

    if(!init())
        return false;

    if(!initialSequence())
        return false;

    start();
    return true;
}

bool QNanoDmx::close()
{
    if(!isOpen())
        return true;

    gracefullyStop();

    return _device->close();
}

bool QNanoDmx::isOpen()
{
    return isRunning();
}

void QNanoDmx::newDataCallback()
{
    _newData.unlock();
}

void QNanoDmx::run()
{
    QElapsedTimer time;

    bool first = true;
    QByteArray compare = _device->readOutputData(0);

    // Wait for device to settle in case the device was opened just recently
    QThread::usleep(1000);

    while(!isInterruptionRequested())
    {
        time.restart();

        if(!_newData.tryLock(0))
        {
            QThread::yieldCurrentThread();
            continue;
        }

        auto dmx = _device->readOutputData(0);

        for(int i = 0; i < dmx.size(); i++)
        {
            if(!first && dmx[i] == compare[i])
                continue;

            quint8 command = i < 256 ? 0xE2 : 0xE3;
            quint8 channel = i < 256 ? (i & 0xFF) : ((i - 256) & 0xFF);

            QByteArray data;
            data.append(command);
            data.append(channel);
            data.append(dmx[i]);

            if(!_device->write(data))
            {
                qWarning() << "[nanodmx] Didn't accept the data";
                _device->purgeBuffers();
                continue;
            }

            compare[i] = dmx[i];

            if(!checkReply())
                _device->purgeBuffers();

            frameSleep(time.elapsed());
        }
    }
}

bool QNanoDmx::initialSequence()
{
    if(!_device->write("C?"))
    {
        qWarning() << "[nanodmx] Initialisation failed";
        return false;
    }

    if(!checkReply())
    {
        qWarning() << "[nanodmx] Initialisation failed";
        return false;
    }

    if(!_device->write("N511"))
    {
        qWarning() << "[nanodmx] Initialisation failed";
        return false;
    }

    if(!checkReply())
    {
        qWarning() << "[nanodmx] Initialisation failed";
        return false;
    }

    return true;
}

bool QNanoDmx::checkReply()
{
    bool ok = false;
    quint8 byte;

    byte = _device->readByte(&ok);

    return ok && byte == 0x47;
}


