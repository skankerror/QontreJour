#ifndef QE131DRIVER_H
#define QE131DRIVER_H

#include "qdmxgenericnetworkdriver.h"

class QE131DriverPrivate;
class Q_DECL_EXPORT QE131Driver : public QDmxGenericNetworkDriver
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QE131Driver);

public:
    QE131Driver(QDmxManager* parent = nullptr);
    ~QE131Driver() override = default;

    QString name() const override { return QStringLiteral("e131"); }

    QString sourceName() const;
    void setSourceName(const QString& n);

protected:
    QDmxGenericNetworkDevice* createDevice(const QNetworkInterface& iface,
                                           const QNetworkAddressEntry& entry) override;
};

#endif // QE131DRIVER_H
