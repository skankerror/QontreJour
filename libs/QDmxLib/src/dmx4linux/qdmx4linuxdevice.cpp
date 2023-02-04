#include <qdmxlib/private/qdmx4linuxdevice.h>
#include <qdmxlib/private/qdmx4linuxdriver.h>
#include <qdmxlib/private/qdmxdevice_p.h>

#include <QFile>
#include <QDebug>

class QDmx4LinuxDevicePrivate : public QDmxDevicePrivate
{
    Q_DECLARE_PUBLIC(QDmx4LinuxDevice);

    Q_DECLARE_DMX_DRIVER(QDmx4LinuxDriver);

public:
    QDmx4LinuxDevicePrivate(QDmxDriver* driver) :
        QDmxDevicePrivate(driver),
        _dmx("/dev/dmx")
    {
        _outputCount = 1;
        _inputCount = 0;
    }

    QFile _dmx;
};

QDmx4LinuxDevice::QDmx4LinuxDevice(QDmx4LinuxDriver* parent) :
    QDmxDevice(*new QDmx4LinuxDevicePrivate(parent), parent)
{
}

void QDmx4LinuxDevice::setData(quint8 port, quint16 channel, quint8 data)
{
    QDmxDevice::setData(port, channel, data);
    sendDmx();
}

void QDmx4LinuxDevice::setData(quint8 port, quint16 channel, const QByteArray& data)
{
    QDmxDevice::setData(port, channel, data);
    sendDmx();
}

void QDmx4LinuxDevice::setData(quint8 port, const QByteArray& data)
{
    QDmxDevice::setData(port, data);
    sendDmx();
}

void QDmx4LinuxDevice::sendDmx()
{
    Q_D(QDmx4LinuxDevice);
    d->_dmx.seek(0);
    if(d->_dmx.write(d->_outputData) != d->_outputData.size())
        qWarning() << "[dmx4linux] Unable to write data:" << d->_dmx.errorString();
}

bool QDmx4LinuxDevice::startHook()
{
    Q_D(QDmx4LinuxDevice);
    if(d->_dmx.open(QIODevice::WriteOnly | QIODevice::Unbuffered))
        return QDmxDevice::startHook();
    else
    {
        qWarning() << "[dmx4linux] Unable to open device:" << d->_dmx.errorString();
        return false;
    }
}

bool QDmx4LinuxDevice::stopHook()
{
    Q_D(QDmx4LinuxDevice);
    d->_dmx.close();
    d->_dmx.unsetError();

    return QDmxDevice::stopHook();
}
