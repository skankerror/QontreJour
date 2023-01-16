#ifndef QDMXBACKEND_H
#define QDMXBACKEND_H

#include <QByteArray>
#include "qdmxusbdevice.h"

class QDmxUsbBackend
{
public:
    QDmxUsbBackend(QDmxUsbDevicePrivate* device);
    virtual ~QDmxUsbBackend();

    virtual QDmxUsbDevice::Backend backend() const  = 0;

    virtual QByteArray readLabel(quint8 label, int& code) = 0;

    virtual bool open() = 0;
    virtual bool openPID(int pid) = 0;
    virtual bool close() = 0;
    virtual bool isOpen() = 0;
    virtual bool reset() = 0;
    virtual bool setLineProperty() = 0;
    virtual bool setBaudRate() = 0;
    virtual bool setFlowControl() = 0;
    virtual bool setLowLatency(bool lowLatency) = 0;
    virtual bool clearRts() = 0;
    virtual bool purgeBuffers() = 0;
    virtual bool setBreak(bool on) = 0;
    virtual bool write(const QByteArray& data) = 0;
    virtual QByteArray read(int size = -1) = 0;
    virtual uchar readByte(bool* ok = nullptr) = 0;

    static bool checkIds(quint16 vid, quint16 pid);
    static bool dealWith(QList<QDmxUsbDevice*>& devices,
                         QList<QDmxUsbDevice*>& tmp,
                         const QString& name,
                         const QString& serial,
                         const QString& vendor,
                         quint16 vid,
                         quint16 pid,
                         QDmxUsbDevice::Backend backend,
                         QDmxUsbDriver* parent);
    static bool clear(QDmxUsbDevice::Backend backend,
                      QList<QDmxUsbDevice*>& devices,
                      QList<QDmxUsbDevice*>& tmp);

protected:
    QDmxUsbDevicePrivate* _d = nullptr;
};

#endif // QDMXBACKEND_H
