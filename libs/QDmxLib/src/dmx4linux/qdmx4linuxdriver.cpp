#include <qdmxlib/private/qdmx4linuxdriver.h>
#include <qdmxlib/private/qdmx4linuxdevice.h>
#include <qdmxlib/private/qdmxdriver_p.h>

#include <QFile>

class QDmx4LinuxDriverPrivate : public QDmxDriverPrivate
{
    Q_DECLARE_PUBLIC(QDmx4LinuxDriver)

public:
    QDmx4LinuxDriverPrivate(QDmxManager* manager) :
        QDmxDriverPrivate(manager) {}

    QDmx4LinuxDevice* _device = nullptr;
};

QDmx4LinuxDriver::QDmx4LinuxDriver(QDmxManager* parent) :
    QDmxDriver(*new QDmx4LinuxDriverPrivate(parent), parent)
{

}

QList<QDmxDevice*> QDmx4LinuxDriver::availableDevices() const
{
    Q_D(const QDmx4LinuxDriver);

    if(d->_device)
        return { d_func()->_device };
    else
        return {};
}

bool QDmx4LinuxDriver::loadHook()
{
    Q_D(QDmx4LinuxDriver);

    if(QFile::exists("/dev/dmx"))
        d->_device = new QDmx4LinuxDevice(this);

    return QDmxDriver::loadHook();
}

bool QDmx4LinuxDriver::unloadHook()
{
    Q_D(QDmx4LinuxDriver);

    if(d->_device)
        d->_device->deleteLater();
    d->_device = nullptr;

    return true;
}
