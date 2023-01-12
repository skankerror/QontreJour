#ifndef QVINCEUSBDMX_H
#define QVINCEUSBDMX_H

#include "qdmxusbinterface.h"

class QVinceUsbDmx : public QDmxUsbInterface
{
public:
    QVinceUsbDmx(QDmxUsbDevice* device);

    bool open() override;
    bool close() override;
    bool isOpen() override;

    void newDataCallback() override;

protected:
    bool sendCommand(quint8 command, const QByteArray& data = {});
    bool checkRely();

protected:
    QByteArray _compare;
};

#endif // QVINCEUSBDMX_H
