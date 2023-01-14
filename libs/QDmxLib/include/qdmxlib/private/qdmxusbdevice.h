#ifndef QDMXUSBDEVICE_H
#define QDMXUSBDEVICE_H

#include "qdmxdevice.h"
#include "qdmxmanager.h"
#include <QDebug>

class QDmxUsbDriver;
class QDmxUsbDevicePrivate;
class Q_DECL_EXPORT QDmxUsbDevice : public QDmxDevice
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxUsbDevice);

    friend class QDmxUsbInterface;
    friend class QOpenDmx;
    friend class QEnttecPro;
    friend class QEnttecProInput;
    friend class QEuroliteUsbDmxPro;
    friend class QNanoDmx;
    friend class QVinceUsbDmx;

public:
    enum Type
    {
        ProRXTX,
        OpenTX,
        ProMk2,
        UltraPro,
        DMX4ALL,
        VinceTX,
        Eurolite
    };
    Q_ENUM(Type)

    enum Backend
    {
        LibFTDI,
        FTD2XX,
        Serial
    };
    Q_ENUM(Backend)

public:
    QDmxUsbDevice(const QString& name,
                  const QString& serial,
                  const QString& vendor,
                  quint16 vid,
                  quint16 pid,
                  QDmxUsbDriver* parent = nullptr);
    ~QDmxUsbDevice() override;

    virtual Backend backend() const = 0;

    QString name() const override;
    QString officialName() const;
    Type type() const;
    QString serialNumber() const;
    QString vendor() const;
    quint16 vendorId() const;
    quint16 productId() const;

protected slots:
    void setData(quint8 port, quint16 channel, quint8 data) override;
    void setData(quint8 port, quint16 channel, const QByteArray& data) override;
    void setData(quint8 port, const QByteArray& data) override;

protected:
    QDmxUsbDevice(QDmxUsbDevicePrivate& d, QDmxUsbDriver* parent);

    bool startHook() override;
    bool stopHook() override;

    void updateInput(quint8 port, const QByteArray& data);

    virtual QByteArray readLabel(quint8 label, int& code) = 0;

    virtual bool open() = 0;
    virtual bool openPID(int pid) = 0;
    virtual bool close() = 0;
    virtual bool isOpen() = 0;
    virtual bool reset() = 0;
    virtual bool setLineProperty() = 0;
    virtual bool setBaudRate() = 0;
    virtual bool setFlowControl() = 0;
    virtual bool setLowLatency(bool lowLatency) = 0;
    virtual bool clearRts() = 0;
    virtual bool purgeBuffers() = 0;
    virtual bool setBreak(bool on) = 0;
    virtual bool write(const QByteArray& data) = 0;
    virtual QByteArray read(int size = -1) = 0;
    virtual uchar readByte(bool* ok = nullptr) = 0;
};



#endif // QDMXUSBDEVICE_H
