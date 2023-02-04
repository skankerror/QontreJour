#ifndef QDMXFTD2XXBACKEND_H
#define QDMXFTD2XXBACKEND_H

#include "qdmxusbbackend_p.h"

class QDmxUsbDriver;
class QDmxUsbDevice;

class QDmxFTD2XXBackend : public QDmxUsbBackend
{
    using ft_handle = void*;

public:
    QDmxFTD2XXBackend(QDmxUsbDevicePrivate* device);
    ~QDmxFTD2XXBackend() override;

    inline QDmxUsbDevice::Backend backend() const override { return QDmxUsbDevice::FTD2XX; }

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
    static quint32 getInterfaceInfo(quint32 devId, QString& vendor, QString& descr,
                                    QString& serial, quint16& vid, quint16& pid);

protected:
    ft_handle _handle = nullptr;
    quint8 _latency = 16;
};

#endif // QDMXFTD2XXBACKEND_H
