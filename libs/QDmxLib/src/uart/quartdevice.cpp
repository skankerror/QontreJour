#include <qdmxlib/private/quartdevice.h>
#include <qdmxlib/private/qdmxdevice_p.h>
#include <qdmxlib/private/quartdriver.h>

#include <QThread>
#include <QSerialPort>
#include <QDebug>

constexpr const int frameFreq = 30;
constexpr const int frameMs = 1000/frameFreq;
constexpr const int breakLength = 110;
constexpr const int mabLength = 16;

class QUartThread : public QThread
{
public:
    QUartThread(const QSerialPortInfo& info, QUartDevice* parent) :
        QThread(parent),
        _device(parent),
        _info(info),
        _port(new QSerialPort(_info, this))
    {}

    void run() override;

protected:
    QUartDevice* _device;
    QSerialPortInfo _info;
    QSerialPort* _port;
};

class QUartDevicePrivate : public QDmxDevicePrivate
{
    Q_DECLARE_PUBLIC(QUartDevice);

public:
    QUartDevicePrivate(const QSerialPortInfo& info, QUartDriver* driver) :
        QDmxDevicePrivate(driver),
        _info(info)
    {
        _outputCount = 1;
        _inputCount = 0;
    }

    QUartThread* _thread = nullptr;
    QSerialPortInfo _info;
};

void QUartThread::run()
{
    if(!_port->open(QIODevice::ReadWrite))
    {
        qWarning() << "[uart]" << _info.portName() << "Unable to open port";
        return;
    }

    if(!_port->clear())
    {
        qWarning() << "[uart]" << _info.portName() << "Unable to clear buffers";
        return;
    }

    if(!_port->setRequestToSend(false))
    {
        qWarning() << "[uart]" << _info.portName() << "Unable to clear request to send";
        return;
    }

    if(!_port->setDataBits(QSerialPort::Data8))
    {
        qWarning() << "[uart]" << _info.portName() << "Unable to set word size";
        return;
    }

    if(!_port->setStopBits(QSerialPort::TwoStop))
    {
        qWarning() << "[uart]" << _info.portName() << "Unable to set stop bits";
        return;
    }

    if(!_port->setParity(QSerialPort::NoParity))
    {
        qWarning() << "[uart]" << _info.portName() << "Unable to set parity";
        return;
    }

    if(!_port->setFlowControl(QSerialPort::NoFlowControl))
    {
        qWarning() << "[uart]" << _info.portName() << "Unable to set flow control";
        return;
    }

    if(!_port->setBaudRate(250000))
    {
        qWarning() << "[uart]" << _info.portName() << "Unable to set baud rate";
        return;
    }

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

        if (_port->setBreakEnabled(true))
        {
            if(goodGranularity)
                QThread::usleep(breakLength);

            if(_port->setBreakEnabled(false))
            {
                if(goodGranularity)
                    QThread::usleep(mabLength);

                _port->write(_device->readOutputData(0));
                _port->waitForBytesWritten(10);
            }
        }

        // Sleep for the remainder of the DMX frame time
        if (goodGranularity)
            while (time.elapsed() < frameMs) { QThread::usleep(1000); }
        else
            while (time.elapsed() < frameMs) { /* Busy sleep */ }
    }
}

QUartDevice::QUartDevice(const QSerialPortInfo& info, QUartDriver* parent) :
    QDmxDevice(*new QUartDevicePrivate(info, parent), parent)
{
}

QString QUartDevice::name() const
{
    return d_func()->_info.portName();
}

bool QUartDevice::startHook()
{
    Q_D(QUartDevice);

    d->_thread = new QUartThread(d->_info, this);
    d->_thread->start(QThread::TimeCriticalPriority);

    return QDmxDevice::stopHook();
}

bool QUartDevice::stopHook()
{
    Q_D(QUartDevice);

    d->_thread->requestInterruption();
    if(!d->_thread->wait(10000))
    {
        d->_thread->terminate();
        d->_thread->wait(10000);
    }

    return QDmxDevice::stopHook();
}
