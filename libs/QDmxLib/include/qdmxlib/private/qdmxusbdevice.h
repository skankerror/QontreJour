#ifndef QDMXUSBDEVICE_H
#define QDMXUSBDEVICE_H

#include "qdmxdevice.h"
#include <QDebug>

class QDmxUsbBackend;
class QDmxUsbDriver;
class QDmxUsbDevicePrivate;
class Q_DECL_EXPORT QDmxUsbDevice : public QDmxDevice
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxUsbDevice);

    friend class QDmxSerialBackend;
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
                  Backend backend,
                  QDmxUsbDriver* parent = nullptr);
    ~QDmxUsbDevice() override;

    Backend backend() const;

    QString name() const override;
    QString officialName() const;
    Type type() const;
    QString serialNumber() const;
    QString vendor() const;
    quint16 vendorId() const;
    quint16 productId() const;

protected:
    QDmxUsbBackend* privateBackend() const;

protected slots:
    void setData(quint8 port, quint16 channel, quint8 data) override;
    void setData(quint8 port, quint16 channel, const QByteArray& data) override;
    void setData(quint8 port, const QByteArray& data) override;

protected:
    QDmxUsbDevice(QDmxUsbDevicePrivate& d, QDmxUsbDriver* parent);

    bool startHook() override;
    bool stopHook() override;

    void updateInput(quint8 port, const QByteArray& data);
};



#endif // QDMXUSBDEVICE_H
