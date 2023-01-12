#ifndef QDMXDRIVER_P_H
#define QDMXDRIVER_P_H

#include <QJsonObject>
#include "qdmxdriver.h"

class QDmxDriverPrivate
{
    Q_DECLARE_PUBLIC(QDmxDriver);

public:
    QDmxDriverPrivate(QDmxManager* manager) : _manager(manager) {}

    bool loadConfig();
    bool saveConfig();

    QString configPath() const;

    virtual bool loadConfigHook(const QJsonObject& config) { Q_UNUSED(config) return true; }
    virtual bool saveConfigHook(QJsonObject& config) { Q_UNUSED(config) return true; };

    template<class T>
    QList<QDmxDevice*> convert(const QList<T*>& devices) const
    {
        QList<QDmxDevice*> r;
        for(auto d : devices)
            r << d;
        return r;
    }

    QDmxDriver* q_ptr = nullptr;

    QDmxManager* _manager = nullptr;
    bool _enabled = false;
    bool _loaded = false;
};

#endif // QDMXDRIVER_P_H
