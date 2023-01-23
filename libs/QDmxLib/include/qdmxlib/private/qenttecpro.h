#ifndef QENTTECPRO_H
#define QENTTECPRO_H

#include "qdmxusbinterface.h"

//#include <QMutex>
#include <QRecursiveMutex>

class QEnttecPro;
class QEnttecProInput : public QDmxUsbThread
{
    Q_OBJECT

    friend class QEnttecPro;

protected:
    QEnttecProInput(QEnttecPro* parent);

public:
    void run() override;

signals:
    void readReady(const QByteArray& data);

protected:
    bool readData(QDmxUsbDevice* device, QByteArray& data);
};

class QEnttecPro : public QDmxUsbThread, public QDmxUsbInterface
{
    Q_OBJECT

    friend class QEnttecProInput;

public:
    QEnttecPro(QDmxUsbDevice* device);

    bool open() override;
    bool close() override;
    bool isOpen() override;

    void newDataCallback() override;

    void run() override;

protected:
    QByteArray payload(quint8 port);

protected slots:
    void inputDataReady(const QByteArray& data);

protected:
    QEnttecProInput* _inputThread = nullptr;
//    QMutex _newData;
    QRecursiveMutex _newData;
};

#endif // QENTTECPRO_H
