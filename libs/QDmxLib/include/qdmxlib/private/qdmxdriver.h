#ifndef QDMXDRIVER_H
#define QDMXDRIVER_H

#include "qdmxdevice.h"
#include "qdmxlib_global.h"

class QDmxManager;
class QDmxDriverPrivate;
class Q_DECL_EXPORT QDmxDriver : public QObject
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxDriver)

public:
    QDmxDriver(QDmxManager* parent = nullptr);
    ~QDmxDriver() override;

    virtual QString name() const = 0;

    QDmxManager* manager() const;

    bool isEnabled() const;
    bool isLoaded() const;

    virtual QList<QDmxDevice*> availableDevices() const = 0;

    QDmxDevice* device(const QString& name) const;

public slots:
    void setEnabled(bool en);
    bool load();
    bool unload();

signals:
    void availableDevicesChanged();
    void enableChanged(bool en);
    void loadedChanged(bool st);

protected:
    QDmxDriver(QDmxDriverPrivate& d, QDmxManager* parent);

    std::unique_ptr<QDmxDriverPrivate> d_ptr;

    virtual bool loadHook() { return true; }
    virtual bool unloadHook() { return true; }
};

class QDmxDummyDriverPrivate;
class Q_DECL_EXPORT QDmxDummyDriver : public QDmxDriver
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxDummyDriver)

public:
    QDmxDummyDriver(QDmxManager* parent = nullptr);
    ~QDmxDummyDriver() override = default;

    QString name() const override { return "dummy"; };

    QList<QDmxDevice*> availableDevices() const override;

protected:
    bool loadHook() override;
    bool unloadHook() override;
};

#endif // QDMXDRIVER_H
