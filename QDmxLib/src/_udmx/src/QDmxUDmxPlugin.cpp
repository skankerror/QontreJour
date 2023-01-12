#ifdef Q_OS_WIN

#else
#include <usb.h>
#endif

#include "QDmxUDmxPlugin.h"


QDmxUDmxPlugin::QDmxUDmxPlugin(QObject* parent) :
    QDmxIO(parent)
{
}

QDmxUDmxPlugin::~QDmxUDmxPlugin()
{
    foreach (QDmxUDmxDevice* dev, _deviceList)
    {
        dev->quit();
        dev->wait();
    }

    qDeleteAll(_deviceList);
}

void QDmxUDmxPlugin::init()
{
    usb_init();
    refreshDevices();
}

void QDmxUDmxPlugin::getDevices()
{

}

void QDmxUDmxPlugin::refreshDevices()
{
    struct usb_device* dev;
    struct usb_bus* bus;

    /* Treat all devices as dead first, until we find them again. Those
       that aren't found, get destroyed at the end of this function. */
    QList <QDmxUDmxDevice*> destroyList(_deviceList);

    usb_find_busses();
    usb_find_devices();

    /* Iterate thru all buses */
    for (bus = usb_get_busses(); bus != NULL; bus = bus->next)
    {
        /* Iterate thru all devices in each bus */
        for (dev = bus->devices; dev != NULL; dev = dev->next)
        {
            QDmxUDmxDevice* udev = device(dev);
            if (udev != NULL)
            {
                /* We already have this device and it's still
                   there. Remove from the destroy list and
                   continue iterating. */
                destroyList.removeAll(udev);
                continue;
            }
            /*else if (QDmxUDmxDevice::isUDMXDevice(dev) == true)
            {
                //This is a new device. Create and append.
                udev = new UDMXDevice(dev, this);
                m_devices.append(udev);
            }*/
        }
    }

    /* Destroy those devices that were no longer found. */
    while (destroyList.isEmpty() == false)
    {
        QDmxUDmxDevice* udev = destroyList.takeFirst();
        _deviceList.removeAll(udev);
        delete udev;
    }
}

QDmxUDmxDevice* QDmxUDmxPlugin::device(struct usb_device *dev)
{
    QListIterator<QDmxUDmxDevice*> it(_deviceList);
    while (it.hasNext() == true)
    {
        QDmxUDmxDevice* udev = it.next();
        if (udev->device() == dev)
            return udev;
    }

    return NULL;
}
