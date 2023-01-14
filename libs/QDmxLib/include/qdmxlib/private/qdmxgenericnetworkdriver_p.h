#ifndef QDMXGENERICNETWORKDRIVER_P_H
#define QDMXGENERICNETWORKDRIVER_P_H

#include "qdmxdriver_p.h"
#include "qdmxgenericnetworkdriver.h"
#include "qdmxgenericnetworkdevice.h"

class QDmxGenericNetworkDriverPrivate : public QDmxDriverPrivate
{
    Q_DECLARE_PUBLIC(QDmxGenericNetworkDriver)

public:
    QDmxGenericNetworkDriverPrivate(QDmxManager* manager) : QDmxDriverPrivate(manager) {}

    QList<QDmxGenericNetworkDevice*> _ifaces;
};


#endif // QDMXGENERICNETWORKDRIVER_P_H
