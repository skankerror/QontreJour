#ifndef QDMXESPNetPLUGIN_H
#define QDMXESPNetPLUGIN_H

#include "QDmxESPNetControler.h"
#include <network/QDmxGenericNetworkPlugin.h>

class QDmxESPNetPlugin : public QDmxGenericNetworkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDmxPlugin_iid)
    Q_INTERFACES(QDmxIO)

public:
    QDmxESPNetPlugin(QObject* parent = nullptr) : QDmxGenericNetworkPlugin(parent) {;}

    //===== GENERAL
    virtual QString name() const { return "ESP Net"; }

protected :
    virtual QDmxGenericNetworkControler* getNewControler(QNetworkInterface iface, QNetworkAddressEntry entry)
    {
        return new QDmxESPNetControler(iface, entry);
    }
};

#endif // QDMXESPNetPLUGIN_H
