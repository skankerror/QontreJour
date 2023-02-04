#ifndef QDMX4LINUXDRIVER_H
#define QDMX4LINUXDRIVER_H

#include "qdmxdriver.h"

class QDmx4LinuxDriverPrivate;
class Q_DECL_EXPORT QDmx4LinuxDriver : public QDmxDriver
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmx4LinuxDriver)

public:
    QDmx4LinuxDriver(QDmxManager* parent = nullptr);
    ~QDmx4LinuxDriver() override = default;

    QString name() const override { return QStringLiteral("dmx4linux"); }

    QList<QDmxDevice*> availableDevices() const override;

protected:
    bool loadHook() override;
    bool unloadHook() override;
};

#endif // QDMX4LINUXDRIVER_H
