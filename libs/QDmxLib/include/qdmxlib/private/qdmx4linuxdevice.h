#ifndef QDXM4LINUXDEVICE_H
#define QDXM4LINUXDEVICE_H

#include "qdmxdevice.h"

class QDmx4LinuxDriver;
class QDmx4LinuxDevicePrivate;
class Q_DECL_EXPORT QDmx4LinuxDevice : public QDmxDevice
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmx4LinuxDevice)

public:
    QDmx4LinuxDevice(QDmx4LinuxDriver *parent = nullptr);
    ~QDmx4LinuxDevice() override = default;

    inline QString name() const override { return "Dmx4Linux"; }

protected slots:
    void setData(quint8 port, quint16 channel, quint8 data) override;
    void setData(quint8 port, quint16 channel, const QByteArray& data) override;
    void setData(quint8 port, const QByteArray& data) override;

    void sendDmx();

protected:
    bool startHook() override;
    bool stopHook() override;
};

#endif // QDXM4LINUXDEVICE_H
