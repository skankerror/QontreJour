#ifndef QDMXUSBINTERFACE_H
#define QDMXUSBINTERFACE_H

#include <QThread>

class QDmxUsbDevice;
class QDmxUsbInterface
{
public:
    QDmxUsbInterface(QDmxUsbDevice* device);
    virtual ~QDmxUsbInterface() = default;

    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool isOpen() = 0;

    virtual void newDataCallback() {}

protected:
    bool init();

protected:
    QDmxUsbDevice* _device = nullptr;
};

class QDmxUsbThread : public QThread
{
    Q_OBJECT

public:
    QDmxUsbThread(QDmxUsbDevice* device = nullptr);

    void gracefullyStop();

    void frameSleep(qint64 elapsed);
};

#endif // QDMXUSBINTERFACE_H
