#ifndef QDMXDEVICE_P_H
#define QDMXDEVICE_P_H

#include <QMutex>
#include "qdmxdevice.h"

#define Q_DECLARE_DMX_DRIVER(Class) Class* driver() const { return static_cast<Class*>(_driver); }
#define Q_DR(Class) Class* dr = driver();

class QDmxDevicePrivate
{
    Q_DECLARE_PUBLIC(QDmxDevice);

public:
    QDmxDevicePrivate(QDmxDriver* driver) : _driver(driver) {};
    virtual ~QDmxDevicePrivate() = default;

    void initBuffers();
    void clearBuffers();

    QByteArray readBuffer(const QByteArray& buffer, quint8 port, quint16 length = 512);

    void updateBuffer(QByteArray& buffer, quint8 port, quint16 channel, quint8 data);
    void updateBuffer(QByteArray& buffer, quint8 port, quint16 channel, const QByteArray& data);
    void updateBuffer(QByteArray& buffer, quint8 port, const QByteArray& data);

    QDmxDevice* q_ptr = nullptr;

    QDmxDriver* _driver = nullptr;
    bool _started = false;

    quint8 _inputCount = 0;
    quint8 _outputCount = 0;

    QMutex _inputMutex;
    QMutex _outputMutex;

    QByteArray _inputData;
    QByteArray _outputData;
};

#endif // QDMXDRIVER_P_H
