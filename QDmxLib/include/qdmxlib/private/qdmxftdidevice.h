#ifndef QDMXFTDIDEVICE_H
#define QDMXFTDIDEVICE_H

#include "qdmxusbdevice.h"

class QDmxFtdiDevicePrivate;
class QDmxFtdiDevice : public QDmxUsbDevice
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxFtdiDevice)

public:
    QDmxFtdiDevice(const QString& name,
                   const QString& serial,
                   const QString& vendor,
                   quint16 vid,
                   quint16 pid,
                   QDmxUsbDriver* parent = nullptr);
    ~QDmxFtdiDevice() override = default;

    Backend backend() const override { return LibFTDI; }

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

#endif // QDMXFTDIDEVICE_H
