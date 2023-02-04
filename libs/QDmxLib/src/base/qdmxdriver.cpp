#include <qdmxlib/private/qdmxdriver_p.h>
#include <qdmxlib/private/qdmxdriver.h>
#include <qdmxlib/private/qdmxmanager.h>

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

class QDmxDummyDriverPrivate : public QDmxDriverPrivate
{
    Q_DECLARE_PUBLIC(QDmxDummyDriver)

public:
    QDmxDummyDriverPrivate(QDmxManager* manager) : QDmxDriverPrivate(manager) {}

    QDmxDevice* _device = nullptr;
};

bool QDmxDriverPrivate::loadConfig()
{
    auto path = configPath();

    qDebug() << QStringLiteral("[%1]").arg(q_func()->name()) << "loading configuration at:" << path;

    QJsonObject config;
    if(QFile::exists(path))
    {
        QFile file(path);
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {
            qWarning() << QStringLiteral("[%1]").arg(q_func()->name()) << "Couldn't open config file" << path;
            return false;
        }

        auto data = file.readAll();
        QJsonParseError err;
        auto doc = QJsonDocument::fromJson(data, &err);

        if(err.error != QJsonParseError::NoError)
        {
            qWarning() << QStringLiteral("[%1]").arg(q_func()->name()) << "Error reading config file" << path;
            qWarning() << err.errorString();
            return false;
        }

        config = doc.object();
        _enabled = config.value("enabled").toBool();
    }

    return loadConfigHook(config);
}

bool QDmxDriverPrivate::saveConfig()
{
    auto path = configPath();
    qDebug() << QStringLiteral("[%1]").arg(q_func()->name()) << "saving configuration at:" << path;

    QJsonObject config;
    config["enabled"] = _enabled;

    if(!saveConfigHook(config))
        return false;

    QJsonDocument doc(config);

    QFile file(path);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << QStringLiteral("[%1]").arg(q_func()->name()) << "Couldn't open config file" << path;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));

    return true;
}

QString QDmxDriverPrivate::configPath() const
{
    Q_Q(const QDmxDriver);
    QDir configDir(_manager->configLocation());
    return configDir.absoluteFilePath(q->name() + ".json");
}

QDmxDriver::QDmxDriver(QDmxManager* parent) :
    QObject(parent),
    d_ptr(new QDmxDriverPrivate(parent))
{
    d_func()->q_ptr = this;
}

QDmxDriver::~QDmxDriver()
{

}

QDmxManager* QDmxDriver::manager() const
{
    return d_func()->_manager;
}

bool QDmxDriver::isEnabled() const
{
    Q_D(const QDmxDriver);
    return d->_enabled;
}

bool QDmxDriver::isLoaded() const
{
    Q_D(const QDmxDriver);
    return d->_loaded;
}

QDmxDevice* QDmxDriver::device(const QString& name) const
{
    auto devices = availableDevices();
    auto it = std::find_if(devices.begin(), devices.end(),
                           [&name](QDmxDevice* d){ return d->name() == name; });

    return it == devices.end() ? nullptr : *it;
}

void QDmxDriver::setEnabled(bool en)
{
    Q_D(QDmxDriver);

    qDebug() << QStringLiteral("[%1]").arg(name()) << "has been set to be" << (en ? "enabled" : "disabled");

    if(en != d->_enabled)
    {
        d->_enabled = en;

        if(en)
            loadHook();
        else
            unloadHook();

        emit enableChanged(en);
    }
}

bool QDmxDriver::load()
{
    Q_D(QDmxDriver);

    qDebug() << QStringLiteral("[%1]").arg(name()) << "loading...";

    if(d->_loaded)
    {
        qWarning() << QStringLiteral("[%1]").arg(name()) << "driver is already loaded.";
        return true;
    }

    if(d->_manager->configEnabled() && !d->loadConfig())
    {
        qWarning() << QStringLiteral("[%1]").arg(name()) << "driver failed to load from config.";
        return false;
    }

    if(d->_enabled)
    {
        if(!loadHook())
            return false;
    }
    else
        qWarning() << QStringLiteral("[%1]").arg(name()) << "driver is disabled.";

    d->_loaded = true;
    emit loadedChanged(true);
    return true;
}

bool QDmxDriver::unload()
{
    Q_D(QDmxDriver);

    qDebug() << QStringLiteral("[%1]").arg(name()) << "unloading...";

    if(!d->_loaded)
    {
        qWarning() << QStringLiteral("[%1]").arg(name()) << "driver is not loaded.";
        return true;
    }

    if(d->_manager->configEnabled() && !d->saveConfig())
        qWarning() << QStringLiteral("[%1]").arg(name()) << "driver failed to save to config.";

    if(d->_enabled)
    {
        if(!unloadHook())
            return false;
    }

    d->_loaded = false;
    emit loadedChanged(false);
    return true;
}

QDmxDriver::QDmxDriver(QDmxDriverPrivate& d, QDmxManager* parent) :
    QObject(parent),
    d_ptr(&d)
{
    d.q_ptr = this;
}


QDmxDummyDriver::QDmxDummyDriver(QDmxManager* parent) :
    QDmxDriver(*new QDmxDummyDriverPrivate(parent), parent)
{
}

QList<QDmxDevice*> QDmxDummyDriver::availableDevices() const
{
    Q_D(const QDmxDummyDriver);
    if(d->_enabled)
        return { d->_device };
    else
        return {};
}

bool QDmxDummyDriver::loadHook()
{
    d_func()->_device = new QDmxDummyDevice(this);
    return QDmxDriver::loadHook();
}

bool QDmxDummyDriver::unloadHook()
{
    d_func()->_device->deleteLater();
    d_func()->_device = nullptr;
    return QDmxDriver::unloadHook();
}
