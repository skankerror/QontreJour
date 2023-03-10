#ifndef QDMXFTDIBACKEND_H
#define QDMXFTDIBACKEND_H

#include "qdmxusbbackend_p.h"

class QDmxUsbDriver;
class QDmxUsbDevice;
struct ftdi_context;

class QDmxFTDIBackend : public QDmxUsbBackend
{
public:
    QDmxFTDIBackend(QDmxUsbDevicePrivate* device);
    ~QDmxFTDIBackend() override;

    inline QDmxUsbDevice::Backend backend() const override { return QDmxUsbDevice::LibFTDI; }

    static bool pollDevices(QList<QDmxUsbDevice*>& devices, QDmxUsbDriver* parent);

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
    ftdi_context* _context = nullptr;
    quint8 _latency = 16;
};

#endif // QDMXFTDIBACKEND_H
