#ifndef QDMXUSBDRIVER_H
#define QDMXUSBDRIVER_H

#include <qdmxlib/QDmxDriver>

class QDmxUsbDriverPrivate;
class Q_DECL_EXPORT QDmxUsbDriver : public QDmxDriver
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxUsbDriver)

public:
    QDmxUsbDriver(QDmxManager* parent = nullptr);
    ~QDmxUsbDriver() override = default;

    QString name() const override { return "dmxusb"; }

    QList<QDmxDevice*> availableDevices() const override;

protected slots:
    void pollDevices(bool preventEmit = false);

protected:
    QDmxUsbDriver(QDmxUsbDriverPrivate& d, QDmxManager* parent);

    bool loadHook() override;
    bool unloadHook() override;
};

#endif // QDMXUSBDRIVER_H
