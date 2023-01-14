/*
  Inspired from QLC Plus sources

  Copyright (c) Massimo Callegari
*/



#ifndef QDMXESPNetPACKETIZER_H
#define QDMXESPNetPACKETIZER_H

#define ESPP 0
#define ESPR 1
#define ESAP 2
#define ESDD 3
#define ESZZ 4

#include <QHostAddress>
#include <QByteArray>
#include <QString>
#include <QHash>

typedef struct {
    QString mac;
    quint16 type;
    quint8  version;
} ESPNetNodeInfo;

class QDmxESPNetPacketizer
{
    /*********************************************************************
     * Initialization
     *********************************************************************/
public:
    QDmxESPNetPacketizer();
    ~QDmxESPNetPacketizer();

public:
    /*********************************************************************
     * Sender functions
     *********************************************************************/
    /** Prepare an ESPNetPoll packet */
    void setupESPNetPoll(QByteArray& data);

    /** Prepare an ESPNetPollReply packet */
    void setupESPNetPollReply(QByteArray &data, ESPNetNodeInfo& info);

    /** Prepare an ESPNetACK packet */
    void setupESPNetACK(QByteArray &data);

    /** Prepare an ESPNetDmx packet */
    void setupESPNetDmx(QByteArray& data, const quint32 &universe, const QByteArray &values);

    /*********************************************************************
     * Receiver functions
     *********************************************************************/

    /** Verify the validity of an ESPNet packet and store the HEAD in 'code' */
    bool checkPacketAndCode(QByteArray const& data, int &code);

    bool fillPollReplyInfo(QByteArray const& data, ESPNetNodeInfo& info);

    bool fillDMXdata(QByteArray const& data, QByteArray& dmx, quint32 &universe);
};

#endif
