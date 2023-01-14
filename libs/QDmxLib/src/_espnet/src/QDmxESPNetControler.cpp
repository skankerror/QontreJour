#include "QDmxESPNetControler.h"
#include <QNetworkDatagram>

QDmxESPNetControler::QDmxESPNetControler(QNetworkInterface interface, QNetworkAddressEntry entry, QObject *parent) :
    QDmxGenericNetworkControler(interface, entry, parent),
    _s(nullptr),
    _packetizer(new QDmxESPNetPacketizer)
{

}

QDmxESPNetControler::~QDmxESPNetControler()
{
    delete _packetizer;
}

void QDmxESPNetControler::readDatagram()
{
    while(_s->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = _s->receiveDatagram();
        if(datagram.senderAddress().isInSubnet(_address, _entry.prefixLength()))
        {
            QByteArray rawData = datagram.data();

            int code;
            if(_packetizer->checkPacketAndCode(rawData, code))
            {
                if(code == ESPP)
                    sendPollReply();
                if(code == ESDD)
                {
                    QByteArray dmx;
                    quint32 u;
                    _packetizer->fillDMXdata(rawData, dmx, u);

                    if(_listenedUniverse.contains(u))
                        emit hasDmx(u, dmx);
                }
            }
        }
    }
}

void QDmxESPNetControler::sendPollReply()
{
    QByteArray data;
    ESPNetNodeInfo info;
    info.mac = _mac;
    info.type = 0x0061;
    info.version = 0x01;
    _packetizer->setupESPNetPollReply(data, info);
    if(socketIsOk())
        _s->writeDatagram(data, _broadcast, ESPNET_PORT);
}

bool QDmxESPNetControler::listenToUniverse(quint32 u)
{
    if(_listenedUniverse.contains(u))
       return true;

    socketNeeded();

    if(!socketIsOk())
        return false;

    _listenedUniverse << u;
    qDebug() << "QDmxESPNetPlugin : listening to universe " << u << " at entry " << _address;

    return true;
}

bool QDmxESPNetControler::ignoreListenedUniverse(quint32 u)
{
    _listenedUniverse.removeAll(u);

    socketUnneeded();

    return true;
}

bool QDmxESPNetControler::writeToUniverse(quint32 u)
{
    if(_writtenUniverse.contains(u))
       return true;

    socketNeeded();

    if(!socketIsOk())
        return false;

    _writtenUniverse << u;
    return true;
}

bool QDmxESPNetControler::ignoreWrittenUniverse(quint32 u)
{
    _writtenUniverse.removeAll(u);

    socketUnneeded();

    return true;
}

void QDmxESPNetControler::write(quint32 u, QByteArray data)
{
    if(_writtenUniverse.contains(u) && data.length() <= 512)
    {
        QByteArray dmxPacket;

        _packetizer->setupESPNetDmx(dmxPacket, u, data);

        if(socketIsOk())
        {
            qint64 r = _s->writeDatagram(dmxPacket, _broadcast, ESPNET_PORT);
            if(r < 0)
            {
                qDebug() << "ESPNet Send Dmx Error";
                qDebug() << _s->error();
                qDebug() << _s->errorString();
            }
        }
    }
}

void QDmxESPNetControler::socketNeeded()
{
    if(socketIsOk())
        return;

    if(!_s)
    {
        _s = new QUdpSocket(this);

        connect(_s, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
              [=](QAbstractSocket::SocketError socketError){
            qDebug() << "Socket Error! : " << socketError << _s->errorString();
        });
        connect(_s, &QUdpSocket::readyRead, this, &QDmxESPNetControler::readDatagram);
    }

    if(_s)
        _s->bind(QHostAddress::AnyIPv4, ESPNET_PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    if(socketIsOk())
        sendPollReply();
}

void QDmxESPNetControler::socketUnneeded()
{
    if(_listenedUniverse.size() == 0 && _writtenUniverse.size() == 0)
    {
        delete _s;
        _s = nullptr;
    }
}
