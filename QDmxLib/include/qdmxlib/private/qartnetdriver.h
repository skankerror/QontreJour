#ifndef QARTNETDRIVER_H
#define QARTNETDRIVER_H

#include "qdmxgenericnetworkdriver.h"

class QArtnetDriverPrivate;
class Q_DECL_EXPORT QArtnetDriver : public QDmxGenericNetworkDriver
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QArtnetDriver)

public:
    enum StyleCode
    {
        Node       = 0x00,
        Controller = 0x01,
        Media      = 0x02,
        Route      = 0x03,
        Backup     = 0x04,
        Config     = 0x05,
        Visual     = 0x06
    };
    Q_ENUM(StyleCode)

public:
    QArtnetDriver(QDmxManager* parent = nullptr);
    ~QArtnetDriver() override = default;

    QString name() const override { return QStringLiteral("artnet"); }

    quint16 artnetAddress(const QString& iface) const;

    QString shortName() const;
    void setShortName(const QString& n);

    QString longName() const;
    void setLongName(const QString& n);

    StyleCode style() const;
    void setStyle(StyleCode c);

protected:
    QDmxGenericNetworkDevice* createDevice(const QNetworkInterface& iface,
                                           const QNetworkAddressEntry& entry) override;
};

#endif // QARTNETDRIVER_H
