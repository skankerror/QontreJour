#ifndef QOPENDMX_H
#define QOPENDMX_H

#include "qdmxusbinterface.h"

class QOpenDmx : public QDmxUsbThread, public QDmxUsbInterface
{
public:
    QOpenDmx(QDmxUsbDevice* device);

    bool open() override;
    bool close() override;
    bool isOpen() override;

    void run() override;
};

#endif // QOPENDMX_H
