#include <qdmxlib/private/qdmxdevice_p.h>
#include <qdmxlib/private/qdmxdriver.h>

#include <QMutexLocker>
#include <QDebug>

class QDmxDummyDevicePrivate : public QDmxDevicePrivate
{
    Q_DECLARE_PUBLIC(QDmxDummyDevice);

public:
    QDmxDummyDevicePrivate(QDmxDriver* driver) :
        QDmxDevicePrivate(driver)
    {
        _inputCount = 1;
        _outputCount = 1;
    };
};

void QDmxDevicePrivate::initBuffers()
{
    _inputData = QByteArray(_inputCount*512, 0);
    _outputData = QByteArray(_outputCount*512, 0);
}

void QDmxDevicePrivate::clearBuffers()
{
    _inputData.clear();
    _outputData.clear();
}

QByteArray QDmxDevicePrivate::readBuffer(const QByteArray& buffer, quint8 port, quint16 length)
{
    Q_ASSERT(port*512+length <= buffer.size());
    return buffer.mid(port*512, length);
}

void QDmxDevicePrivate::updateBuffer(QByteArray& buffer, quint8 port, quint16 channel, quint8 data)
{
    Q_ASSERT(port*512+channel < buffer.size());

    buffer[port*512 + channel] = data;
}

void QDmxDevicePrivate::updateBuffer(QByteArray& buffer, quint8 port, quint16 channel, const QByteArray& data)
{
    Q_ASSERT(port*512+channel+data.size() <= buffer.size());
    buffer.replace(port*512 + channel, data.size(), data);
}

void QDmxDevicePrivate::updateBuffer(QByteArray& buffer, quint8 port, const QByteArray& data)
{
    updateBuffer(buffer, port, 0, data);
}

QDmxDevice::QDmxDevice(QDmxDriver* parent) :
    QObject(parent),
    d_ptr(new QDmxDevicePrivate(parent))
{
    d_func()->q_ptr = this;
}

QDmxDevice::~QDmxDevice()
{

}

QDmxDriver* QDmxDevice::driver() const
{
    return d_func()->_driver;
}

QDmxManager* QDmxDevice::manager() const
{
    return driver()->manager();
}

bool QDmxDevice::isStarted() const
{
    return d_func()->_started;
}

quint8 QDmxDevice::inputCount() const
{
    return d_func()->_inputCount;
}

quint8 QDmxDevice::outputCount() const
{
    return d_func()->_outputCount;
}

QByteArray QDmxDevice::readInputData(quint8 port)
{
    Q_D(QDmxDevice);
    QMutexLocker locker(&d->_inputMutex);
    return d->readBuffer(d->_inputData, port);
}

QByteArray QDmxDevice::readOutputData(quint8 port)
{
    Q_D(QDmxDevice);
    QMutexLocker locker(&d->_outputMutex);
    return d->readBuffer(d->_outputData, port);
}

bool QDmxDevice::start()
{
    Q_D(QDmxDevice);

    if(d->_started)
    {
        qWarning() << "[device]" << name() << "is already started.";
        return true;
    }

    d->initBuffers();

    if(!startHook())
        return false;

    d->_started = true;
    return true;
}

bool QDmxDevice::stop()
{
    Q_D(QDmxDevice);

    if(!d->_started)
    {
        qWarning() << "[device]" << name() << "is already stopped.";
        return true;
    }

    if(!stopHook())
        return false;

    d->clearBuffers();

    d->_started = false;
    return true;
}

void QDmxDevice::setData(quint8 port, quint16 channel, quint8 data)
{
    Q_D(QDmxDevice);
    QMutexLocker locker(&d->_outputMutex);
    d->updateBuffer(d->_outputData, port, channel, data);
}

void QDmxDevice::setData(quint8 port, quint16 channel, const QByteArray& data)
{
    Q_D(QDmxDevice);
    QMutexLocker locker(&d->_outputMutex);
    d->updateBuffer(d->_outputData, port, channel, data);
}

void QDmxDevice::setData(quint8 port, const QByteArray& data)
{
    Q_D(QDmxDevice);
    QMutexLocker locker(&d->_outputMutex);
    d->updateBuffer(d->_outputData, port, data);
}

QDmxDevice::QDmxDevice(QDmxDevicePrivate& d, QDmxDriver* parent) :
    QObject(parent),
    d_ptr(&d)
{
    d.q_ptr = this;
}

QDmxDummyDevice::QDmxDummyDevice(QDmxDriver* parent) :
    QDmxDevice(*new QDmxDummyDevicePrivate(parent), parent)
{
}
