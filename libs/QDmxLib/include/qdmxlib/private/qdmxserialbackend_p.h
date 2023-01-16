#ifndef QDMXSERIALBACKEND_H
#define QDMXSERIALBACKEND_H

#include <QSerialPort>
#include <QSerialPortInfo>

#include "qdmxusbbackend_p.h"

class QDmxUsbDriver;
class QDmxUsbDevice;

class QDmxSerialBackend : public QDmxUsbBackend
{
public:
    QDmxSerialBackend(QDmxUsbDevicePrivate* device);
    ~QDmxSerialBackend() override;

    inline QDmxUsbDevice::Backend backend() const override { return QDmxUsbDevice::Serial; }

    static bool pollDevices(QList<QDmxUsbDevice*>& devices, QDmxUsbDriver* parent);

    void setInfo(const QSerialPortInfo& info);

    QByteArray readLabel(quint8 label, int& code) override;

    bool open() override;
    bool openPID(int pid) override;
    bool close() override;
    bool isOpen() override;
    bool reset() override;
    bool setLineProperty() override;
    bool setBaudRate() override;
    bool setFlowControl() override;
    bool setLowLatency(bool lowLatency) override;
    bool clearRts() override;
    bool purgeBuffers() override;
    bool setBreak(bool on) override;
    bool write(const QByteArray& data) override;
    QByteArray read(int size = -1) override;
    quint8 readByte(bool* ok = nullptr) override;

protected:
    QSerialPortInfo _info;
    QSerialPort* _serial = nullptr;
};

#endif // QDMXSERIALBACKEND_H
