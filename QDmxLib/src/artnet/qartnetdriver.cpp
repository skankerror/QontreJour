#include <qdmxlib/private/qartnetdriver.h>
#include <qdmxlib/private/qartnetdevice.h>
#include <qdmxlib/private/qdmxgenericnetworkdriver_p.h>

class QArtnetDriverPrivate : public QDmxGenericNetworkDriverPrivate
{
    Q_DECLARE_PUBLIC(QArtnetDriver)

public:
    QArtnetDriverPrivate(QDmxManager* manager) :
        QDmxGenericNetworkDriverPrivate(manager) {}

    bool loadConfigHook(const QJsonObject& config) override;
    bool saveConfigHook(QJsonObject& config) override;

    void sendPollReply();

    QString _shortName;
    QString _longName;

    QArtnetDriver::StyleCode _style;

    QMap<QString, quint16> _artnetAddresses;
};

bool QArtnetDriverPrivate::loadConfigHook(const QJsonObject& config)
{
    _shortName = config["short_name"].toString("QDmxLib");
    _longName = config["long_name"].toString("QDmxLib - ArtNet Driver");

    auto s = config["style"].toString("controller");
    if(s == "controller")
        _style = QArtnetDriver::Controller;
    else if(s == "media")
        _style = QArtnetDriver::Media;
    else if(s == "route")
        _style = QArtnetDriver::Route;
    else if(s == "backup")
        _style = QArtnetDriver::Backup;
    else if(s == "config")
        _style = QArtnetDriver::Config;
    else if(s == "visual")
        _style = QArtnetDriver::Visual;
    else
        _style = QArtnetDriver::Node;

    for(auto it = config.begin(); it != config.end(); ++it)
    {
        if(it.key() == "enabled" ||
           it.key() == "short_name" ||
           it.key() == "long_name" ||
           it.key() == "style")
            continue;

        _artnetAddresses.insert(it.key(), it.value().toInt());
    }

    return true;
}

bool QArtnetDriverPrivate::saveConfigHook(QJsonObject& config)
{
    config["short_name"] = _shortName;
    config["long_name"] = _longName;

    switch(_style)
    {
    default:
    case QArtnetDriver::Node:
        config["style"] = "node";
        break;

    case QArtnetDriver::Controller:
        config["style"] = "controller";
        break;

    case QArtnetDriver::Media:
        config["style"] = "media";
        break;

    case QArtnetDriver::Route:
        config["style"] = "route";
        break;

    case QArtnetDriver::Backup:
        config["style"] = "backup";
        break;

    case QArtnetDriver::Config:
        config["style"] = "config";
        break;

    case QArtnetDriver::Visual:
        config["style"] = "visual";
        break;
    }

    for(auto it = _artnetAddresses.begin(); it != _artnetAddresses.end(); ++it)
        config[it.key()] = it.value();

    return true;
}

void QArtnetDriverPrivate::sendPollReply()
{
    for(auto dev : _ifaces)
    {
        if(dev->isStarted())
            static_cast<QArtnetDevice*>(dev)->sendPollReply();
    }
}

QArtnetDriver::QArtnetDriver(QDmxManager* parent) :
    QDmxGenericNetworkDriver(*new QArtnetDriverPrivate(parent), parent)
{

}

quint16 QArtnetDriver::artnetAddress(const QString& iface) const
{
    return d_func()->_artnetAddresses.value(iface, 0);
}

QString QArtnetDriver::shortName() const
{
    return d_func()->_shortName;
}

void QArtnetDriver::setShortName(const QString& n)
{
    Q_D(QArtnetDriver);
    d->_shortName = n;
    d->sendPollReply();
}

QString QArtnetDriver::longName() const
{
    return d_func()->_longName;
}

void QArtnetDriver::setLongName(const QString& n)
{
    Q_D(QArtnetDriver);
    d->_longName = n;
    d->sendPollReply();
}

QArtnetDriver::StyleCode QArtnetDriver::style() const
{
    return d_func()->_style;
}

void QArtnetDriver::setStyle(StyleCode c)
{
    Q_D(QArtnetDriver);
    d->_style = c;
    d->sendPollReply();
}

QDmxGenericNetworkDevice* QArtnetDriver::createDevice(const QNetworkInterface& iface,
                                                      const QNetworkAddressEntry& entry)
{
    return new QArtnetDevice(iface, entry, this);
}
