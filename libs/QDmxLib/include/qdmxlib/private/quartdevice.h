#ifndef QUARTDEVICE_H
#define QUARTDEVICE_H

#include "qdmxdevice.h"

#include <QSerialPortInfo>

class QUartDriver;
class QUartDevicePrivate;
class QUartDevice : public QDmxDevice
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QUartDevice)

public:
    QUartDevice(const QSerialPortInfo& info, QUartDriver* parent = nullptr);

    QString name() const override;

protected:
    bool startHook() override;
    bool stopHook() override;
};

#endif // QUARTDEVICE_H
