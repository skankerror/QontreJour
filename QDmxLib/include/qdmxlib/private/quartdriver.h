#ifndef QUARTDRIVER_H
#define QUARTDRIVER_H

#include "qdmxdriver.h"

class QUartDriverPrivate;
class QUartDriver : public QDmxDriver
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QUartDriver)

public:
    QUartDriver(QDmxManager* parent = nullptr);

    QString name() const override { return "uart"; }

    QList<QDmxDevice*> availableDevices() const override;

protected:
    bool loadHook() override;
    bool unloadHook() override;
};

#endif // QUARTDRIVER_H
