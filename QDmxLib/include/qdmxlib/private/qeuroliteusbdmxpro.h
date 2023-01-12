#ifndef QEUROLITEUSBDMXPRO_H
#define QEUROLITEUSBDMXPRO_H

#include "qdmxusbinterface.h"
#include <QMutex>

class QEuroliteUsbDmxPro : public QDmxUsbThread, public QDmxUsbInterface
{
public:
    QEuroliteUsbDmxPro(QDmxUsbDevice* device);

    bool open() override;
    bool close() override;
    bool isOpen() override;

    void run() override;

    void newDataCallback() override;

protected:
    QMutex _newData;
};

#endif // QEUROLITEUSBDMXPRO_H
