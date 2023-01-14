#ifndef QDMXSERIALDEVICE_H
#define QDMXSERIALDEVICE_H

#include "qdmxusbdevice.h"
#include <QSerialPortInfo>

class QDmxSerialDevicePrivate;
class QDmxSerialDevice : public QDmxUsbDevice
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxSerialDevice)

public:
    QDmxSerialDevice(const QString& name,
                     const QString& serial,
                     const QString& vendor,
                     quint16 vid,
                     quint16 pid,
                     const QSerialPortInfo& info,
                     QDmxUsbDriver* parent = nullptr);
    ~QDmxSerialDevice() override = default;

    Backend backend() const override { return Serial; }

    static bool pollDevices(QList<QDmxUsbDevice*>& devices, QDmxUsbDriver* parent);

protected:
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
};

#endif // QDMXSERIALDEVICE_H
