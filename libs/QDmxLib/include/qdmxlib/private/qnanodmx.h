#ifndef QNANODMX_H
#define QNANODMX_H

#include "qdmxusbinterface.h"
#include <QMutex>

class QNanoDmx : public QDmxUsbThread, public QDmxUsbInterface
{
public:
    QNanoDmx(QDmxUsbDevice* device);

    bool open() override;
    bool close() override;
    bool isOpen() override;

    void newDataCallback() override;

    void run() override;

protected:
    bool initialSequence();
    bool checkReply();

protected:
    QMutex _newData;
};

#endif // QNANODMX_H
