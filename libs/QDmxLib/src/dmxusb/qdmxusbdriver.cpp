#include <qdmxlib/private/qdmxusbdriver.h>
#include <qdmxlib/private/qdmxusbdevice.h>
#include <qdmxlib/private/qdmxmanager.h>
#include <qdmxlib/private/qdmxdriver_p.h>

#include <QTimer>

#ifdef QDMXLIB_HAS_FTDI
#include <qdmxlib/private/qdmxftdibackend_p.h>
#endif

#ifdef QDMXLIB_HAS_QTSERIAL
#include <qdmxlib/private/qdmxserialbackend_p.h>
#endif

class QDmxUsbDriverPrivate : public QDmxDriverPrivate
{
    Q_DECLARE_PUBLIC(QDmxUsbDriver);

public:
    QDmxUsbDriverPrivate(QDmxManager* manager) : QDmxDriverPrivate(manager) {}

    QTimer _pollTimer;
    QList<QDmxUsbDevice*> _devices;
};

QDmxUsbDriver::QDmxUsbDriver(QDmxManager* parent) :
    QDmxDriver(*new QDmxUsbDriverPrivate(parent), parent)
{
    Q_D(QDmxUsbDriver);
    d->_pollTimer.setInterval(2000);
    d->_pollTimer.setTimerType(Qt::VeryCoarseTimer);

    connect(&d->_pollTimer, SIGNAL(timeout()),
            this, SLOT(pollDevices()));
}

QList<QDmxDevice*> QDmxUsbDriver::availableDevices() const
{
    Q_D(const QDmxUsbDriver);
    return d->convert(d->_devices);
}

void QDmxUsbDriver::pollDevices(bool preventEmit)
{
    Q_D(QDmxUsbDriver);

    bool shouldEmit = false;

#ifdef QDMXLIB_HAS_FTDI
    shouldEmit |= QDmxFTDIBackend::pollDevices(d->_devices, this);
#endif

#ifdef QDMXLIB_HAS_QTSERIAL
    shouldEmit |= QDmxSerialBackend::pollDevices(d->_devices, this);
#endif

    if(shouldEmit && !preventEmit)
        emit availableDevicesChanged();
}

QDmxUsbDriver::QDmxUsbDriver(QDmxUsbDriverPrivate& d, QDmxManager* parent) :
    QDmxDriver(d, parent)
{

}

bool QDmxUsbDriver::loadHook()
{
    pollDevices(true);

    d_func()->_pollTimer.start();

    return true;
}

bool QDmxUsbDriver::unloadHook()
{
    Q_D(QDmxUsbDriver);

    for(auto dev : d->_devices)
        dev->deleteLater();
    d->_devices.clear();

    d->_pollTimer.stop();

    return true;
}
