#include <qdmxlib/private/qopendmx.h>
#include <qdmxlib/private/qdmxusbdevice.h>

#include <QElapsedTimer>

constexpr const int frameFreq = 30;
constexpr const int frameMs = 1000/frameFreq;
constexpr const int breakLength = 110;
constexpr const int mabLength = 16;

QOpenDmx::QOpenDmx(QDmxUsbDevice* device) :
    QDmxUsbThread(device),
    QDmxUsbInterface(device)
{
}

bool QOpenDmx::open()
{
    if(isOpen())
        return true;

    if(!init())
        return false;

    start(QThread::TimeCriticalPriority);
    return true;
}

bool QOpenDmx::close()
{
    if(!isOpen())
        return true;

    gracefullyStop();

    return _device->close();
}

bool QOpenDmx::isOpen()
{
    return isRunning();
}

void QOpenDmx::run()
{
    bool goodGranularity = true;

    QElapsedTimer time;
    time.start();

    QThread::usleep(1000);

    if (time.elapsed() > 3)
        goodGranularity = false;

    while(!isInterruptionRequested())
    {
        // Measure how much time passes during these calls
        time.restart();

        if (_device->setBreak(true))
        {
            if(goodGranularity)
                QThread::usleep(breakLength);

            if(_device->setBreak(false))
            {
                if(goodGranularity)
                    QThread::usleep(mabLength);

                _device->write(_device->readOutputData(0));
            }
        }

        // Sleep for the remainder of the DMX frame time
        if (goodGranularity)
            while (time.elapsed() < frameMs) { QThread::usleep(1000); }
        else
            while (time.elapsed() < frameMs) { /* Busy sleep */ }
    }
}
