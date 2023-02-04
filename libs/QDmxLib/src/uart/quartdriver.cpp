#include <qdmxlib/private/quartdriver.h>
#include <qdmxlib/private/quartdevice.h>
#include <qdmxlib/private/qdmxdriver_p.h>

class QUartDriverPrivate : public QDmxDriverPrivate
{
    Q_DECLARE_PUBLIC(QUartDriver);

public:
    QUartDriverPrivate(QDmxManager* manager) : QDmxDriverPrivate(manager) {}

    QList<QUartDevice*> _devices;
};

QUartDriver::QUartDriver(QDmxManager* parent) :
    QDmxDriver(*new QUartDriverPrivate(parent), parent)
{

}

QList<QDmxDevice*> QUartDriver::availableDevices() const
{
    Q_D(const QUartDriver);
    return d->convert(d->_devices);
}

bool QUartDriver::loadHook()
{
    Q_D(QUartDriver);
    for(const auto& info : QSerialPortInfo::availablePorts())
    {
        // Skip ports with vid and pid because they'll be handled by
        // USB Dmx driver
        if(info.hasVendorIdentifier() || info.hasProductIdentifier())
            continue;

        d->_devices << new QUartDevice(info, this);
    }

    return QDmxDriver::loadHook();
}

bool QUartDriver::unloadHook()
{
    Q_D(QUartDriver);

    for(auto dev : d->_devices)
        dev->deleteLater();
    d->_devices.clear();

    return true;
}
