#ifndef QDMXUDMXDEVICE_H
#define QDMXUDMXDEVICE_H

#ifdef Q_OS_WIN

#else
#include <usb.h>
#endif

#include <QThread>

class QDmxUDmxDevice : public QThread
{
public:
    QDmxUDmxDevice();

    struct usb_device* device() { return _dev; }

    void run();

private:
    struct usb_device* _dev;
};

typedef QSharedPointer<QDmxUDmxDevice> QDmxUDmxDevice_ptr;

#endif // QDMXUDMXDEVICE_H
