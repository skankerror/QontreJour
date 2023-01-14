#include <qdmxlib/private/qartnetdevice.h>
#include <qdmxlib/private/qartnetdriver.h>
#include <qdmxlib/private/qdmxgenericnetworkdevice_p.h>

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <array>

constexpr const int     artnet_universe_count   = 4;
constexpr const int     artnet_port             = 6454;
constexpr const quint16 artnet_poll_opcode      = 0x2000;
constexpr const quint16 artnet_pollreply_opcode = 0x2100;
constexpr const quint16 artnet_dmx_opcode       = 0x5000;

class QArtnetDevicePrivate : public QDmxGenericNetworkDevicePrivate
{
    Q_DECLARE_PUBLIC(QArtnetDevice);

    Q_DECLARE_DMX_DRIVER(QArtnetDriver);

public:
    struct ArtnetSubscription {

        ArtnetSubscription() = default;
        ArtnetSubscription(const ArtnetSubscription&) = default;
        ArtnetSubscription(ArtnetSubscription&&) = default;
        ArtnetSubscription(const QHostAddress& a) :
            address(a)
        {
            updateTimer.start();
        }

        ArtnetSubscription& operator=(const ArtnetSubscription&) = default;
        ArtnetSubscription& operator=(ArtnetSubscription&&) = default;

        bool operator ==(const ArtnetSubscription& other) const
        {
            return address == other.address;
        }

        bool operator !=(const ArtnetSubscription& other) const
        {
            return !(*this == other);
        }

        bool operator ==(const QHostAddress& other) const
        {
            return address == other;
        }

        bool operator !=(const QHostAddress& other) const
        {
            return address != other;
        }

        QHostAddress address;
        QElapsedTimer updateTimer;
    };

public:
    QArtnetDevicePrivate(const QNetworkInterface& interface,
                         const QNetworkAddressEntry& entry,
                         QDmxDriver* driver);

    QHostAddress preferedArtnetAddress() const;

    QByteArray artnetHeader(quint16 opcode);
    QByteArray artnetPoll();
    QByteArray artnetPollReply();
    QByteArray artnetDmx(quint8 port, quint16 length = 512);

    bool checkPacket(const QByteArray& data, quint16& code);
    bool extractPollInfo(const QByteArray& data, QString& shortName, QString& longName);
    bool extractDmxData(const QByteArray& data, const QString& ifaceName, quint8& port);

    QUdpSocket* _socket = nullptr;

    QTimer _dmxTimer;
    QTimer _subscriberTimer;

    std::array<quint8, artnet_universe_count> _inputSequence;
    std::array<quint8, artnet_universe_count> _outputSequence;

    QMultiMap<quint8, ArtnetSubscription> _subscribedNodes;
};

QArtnetDevicePrivate::QArtnetDevicePrivate(const QNetworkInterface& interface,
                                           const QNetworkAddressEntry& entry,
                                           QDmxDriver* driver) :
    QDmxGenericNetworkDevicePrivate(interface, entry, driver)
{
    _inputCount = artnet_universe_count;
    _outputCount = artnet_universe_count;

    for(auto& c : _inputSequence)
        c = 1;

    for(auto& c : _outputSequence)
        c = 1;

    _dmxTimer.setInterval(1000);
    _dmxTimer.setTimerType(Qt::VeryCoarseTimer);

    _subscriberTimer.setInterval(10000);
    _subscriberTimer.setTimerType(Qt::VeryCoarseTimer);
}

QHostAddress QArtnetDevicePrivate::preferedArtnetAddress() const
{
    return _address;
}

QByteArray QArtnetDevicePrivate::artnetHeader(quint16 opcode)
{
    QByteArray data("Art-Net");
    data.append('\x00');

    // opcode
    data.append(opcode & 0x00FF);
    data.append((opcode >> 8) & 0x00FF);

    if(opcode == artnet_pollreply_opcode)
    {
        for(const auto& comp : _address.toString().split('.'))
            data.append(comp.toInt());

        data.append('\x36'); // Port LSB
        data.append('\x19'); // Port MSB
    }

    // version 14 by default
    data.append('\x00');
    data.append('\x0e');

    return data;
}

QByteArray QArtnetDevicePrivate::artnetPoll()
{
    QByteArray data = artnetHeader(artnet_poll_opcode);

    data.append('\x02'); // Talk To Me
    data.append('\x10'); // Priority

    return data;
}

QByteArray QArtnetDevicePrivate::artnetPollReply()
{
    Q_DR(QArtnetDriver);

    QByteArray data = artnetHeader(artnet_pollreply_opcode);

    quint16 addr = dr->artnetAddress(q_func()->name());

    data.append((addr >> 8) & 0x007F); // Net
    data.append((addr >> 4) & 0x000F); // Sub

    data.append('\xFF');     // OEM Value MSB
    data.append('\xFF');     // OEM Value LSB

    data.append('\x00');     // UBEA version

    data.append('\xF0');     // Status1 - Ready and booted

    data.append('\xFF');     // ESTA Manufacturer MSB
    data.append('\xFF');     // ESTA Manufacturer LSB

    data.append(dr->shortName().leftJustified(18, '\x00', true).toUtf8()); // Short Name
    data.append(dr->longName().leftJustified(64, '\x00', true).toUtf8());  // Long Name

    for(int i = 0; i < 64; i++)
        data.append('\x00'); // Node report

    data.append('\x00');     // NumPort MSB
    data.append('\x04');     // NumPort LSB

    data.append('\xC0');     // Port 1-4 type: can input and output DMX512 data
    data.append('\xC0');
    data.append('\xC0');
    data.append('\xC0');

    for(int i = 0; i < 8; i++)
        data.append('\x00'); // Set GoodInput[4] and GoodOutput[4] all to unknown state

    for(int i = 0; i < 4; i++)
        data.append(i); // Set SwIn Address

    for(int i = 0; i < 4; i++)
        data.append(i); // Set SwOut Address

    data.append('\x00'); // sACN convertion priority
    data.append('\x00'); // No Macros
    data.append('\x00'); // No Remotes

    data.append('\x00'); // spare
    data.append('\x00');
    data.append('\x00');

    data.append(static_cast<char>(dr->style())); // style

    for(const auto& couple : _mac.split(':'))
        data.append(static_cast<char>(couple.toInt(nullptr, 16))); // mac

    for(const auto& comp : _address.toString().split('.'))
        data.append(comp.toInt()); // bind ip

    data.append(1); // bind index

    data.append(0xC0 + (_entry.isPermanent() ? 0 : 2)); // status 2

    for(int i = 0; i < 4; i++)
        data.append(0x40); // good output

    data.append('\x00'); // status 3

    for(int i = 0; i < 6; i++)
        data.append('\x00'); // RDMnet uid ???

    for(int i = 0; i < 15; i++)
        data.append('\x00'); // Filter

    return data;
}

QByteArray QArtnetDevicePrivate::artnetDmx(quint8 port, quint16 length)
{
    Q_DR(QArtnetDriver);

    QByteArray data = artnetHeader(artnet_dmx_opcode);

    data.append(_outputSequence[port]);
    data.append('\x00'); // Physical

    auto addr = dr->artnetAddress(q_func()->name());

    data.append((addr + port) & 0x00FF);
    data.append((addr >> 8) & 0x007F);

    length = std::clamp(length, static_cast<quint16>(2), static_cast<quint16>(512));
    if((length % 2) == 1)
        length++;

    data.append(length >> 8);
    data.append(length & 0x00FF);

    data.append(readBuffer(_outputData, port, length));

    if (_outputSequence[port] == 0xFF)
        _outputSequence[port] = 1;
    else
        _outputSequence[port]++;

    return data;
}

bool QArtnetDevicePrivate::checkPacket(const QByteArray& data, quint16& code)
{
    /* An ArtNet header must be at least 12 bytes long */
    if (data.length() < 12)
    {
        qWarning() << "[artnet]" << q_func()->name() << "received a packet too short.";
        return false;
    }

    /* Check "Art-Net" keyword presence */
    if (!data.startsWith("Art-Net"))
    {
        qWarning() << "[artnet]" << q_func()->name() << "received an invalid packet.";
        return false;
    }

    if (data.at(7) != 0x00)
    {
        qWarning() << "[artnet]" << q_func()->name() << "received an invalid packet.";
        return false;
    }

    code = (data.at(9) << 8) + data.at(8);

    return true;
}

bool QArtnetDevicePrivate::extractPollInfo(const QByteArray& data,
                                           QString& shortName,
                                           QString& longName)
{
    Q_DR(QArtnetDriver);
    Q_Q(QArtnetDevice);

    if(data.size() < 108)
    {
        qWarning() << "[artnet]" << q_func()->name() << "received an invalid PollReply packet.";
        return false;
    }

    // extract ip address
    quint32 ip = 0;
    for(int i = 0; i < 4; i++)
    {
        quint8 value = data[11+i];
        ip += (value << (24 - (i*8)));
    }

    QHostAddress ipAddr(ip);

    // extract artnet address and subscribe if any relevant outputs

    quint16 artnetAddress = (data[19] << 8) + data[20];

    if(artnetAddress == dr->artnetAddress(q->name()))
    {
        quint8 portCount = data[174];
        for(quint8 i = 0; i < portCount; i++)
        {
            quint8 flags = data[175+i];
            if((flags & 0xBF) == 128)
            {
                quint8 u = data[191];
                if(u < 4)
                {
                    auto it = _subscribedNodes.find(u, ipAddr);
                    if(it != _subscribedNodes.end())
                        it->updateTimer.restart();
                    else
                        _subscribedNodes.insert(u, ipAddr);
                }
            }
        }
    }

    shortName = QString::fromLocal8Bit(data.mid(26, 18).data());
    longName  = QString::fromLocal8Bit(data.mid(44, 64).data());

    return true;
}

bool QArtnetDevicePrivate::extractDmxData(const QByteArray& data, const QString& ifaceName, quint8& port)
{
    Q_DR(QArtnetDriver);

    if(data.size() < 26)
    {
        qWarning() << "[artnet]" << q_func()->name() << "received an invalid ArtDmx packet.";
        return false;
    }

    quint16 recievedAddr = (data[15] << 8) + data[14];
    quint16 addr = dr->artnetAddress(ifaceName);

    if((recievedAddr & 0x7FF0) != addr)
    {
        qWarning() << "[artnet]" << q_func()->name() << "received an ArtDmx packet for the wrong address.";
        return false;
    }

    port = recievedAddr & 0x000F;

    quint8 seq = data[12];
    if(seq <= _inputSequence[port] && _inputSequence[port] != 0xFF)
    {
        qWarning() << "[artnet]" << q_func()->name() << "received an ArtDmx packet with a bad sequence number.";
        return false;
    }

    _inputSequence[port] = seq;

    quint16 length = (data[16] << 8) + data[17];

    auto dmx = data.mid(18, length);

    updateBuffer(_inputData, port, dmx);

    return true;
}

QArtnetDevice::QArtnetDevice(const QNetworkInterface& interface,
                             const QNetworkAddressEntry& entry,
                             QDmxGenericNetworkDriver* parent) :
    QDmxGenericNetworkDevice(*new QArtnetDevicePrivate(interface, entry, parent), parent)
{
    Q_D(QArtnetDevice);
    connect(&d->_dmxTimer, &QTimer::timeout,
            this, &QArtnetDevice::sendDmx);

    connect(&d->_subscriberTimer, &QTimer::timeout,
            this, &QArtnetDevice::checkSubscriptions);
}

void QArtnetDevice::setData(quint8 port, quint16 channel, quint8 data)
{
    QDmxGenericNetworkDevice::setData(port, channel, data);
    d_func()->_dmxTimer.start();
    sendDmx();
}

void QArtnetDevice::setData(quint8 port, quint16 channel, const QByteArray& data)
{
    QDmxGenericNetworkDevice::setData(port, channel, data);
    d_func()->_dmxTimer.start();
    sendDmx();
}

void QArtnetDevice::setData(quint8 port, const QByteArray& data)
{
    Q_ASSERT(port < 4);
    Q_ASSERT(data.size() <= 512);

    setData(port, 0, data);
}

void QArtnetDevice::readDatagram()
{
    Q_D(QArtnetDevice);

    while(d->_socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = d->_socket->receiveDatagram();
        auto senderAddress = datagram.senderAddress();
        if(senderAddress != d->_address && senderAddress.isInSubnet(d->_address, d->_entry.prefixLength()))
        {
            QByteArray rawData = datagram.data();

            quint16 code;
            if(d->checkPacket(rawData, code))
            {
                switch(code)
                {
                case artnet_poll_opcode:
                {
                    qDebug() << "[artnet] Received a Poll from" << senderAddress.toString();
                    int delay = QRandomGenerator::system()->bounded(0, 1000);
                    QTimer::singleShot(delay, this, &QArtnetDevice::sendPollReply);
                    break;
                }

                case artnet_dmx_opcode:
                {
                    quint8 port;
                    if(d->extractDmxData(rawData, name(), port))
                        emit inputDataChanged(port, readInputData(port));
                    break;
                }

                case artnet_pollreply_opcode:
                {
                    QString s, l;
                    if(d->extractPollInfo(rawData, s, l))
                        qDebug() << "[artnet] Received a PollReply:" << s << l;
                    break;
                }

                default:
                    qDebug() << "[artnet] Received an unsupported opcode:" << code;
                    qDebug() << "[artnet]" << rawData;
                    break;
                }
            }
        }
    }
}

void QArtnetDevice::socketError(QAbstractSocket::SocketError socketError)
{
    qWarning() << "[artnet] Socker error:" << socketError;
}

void QArtnetDevice::sendDmx()
{
    Q_D(QArtnetDevice);

    if(!isStarted())
        return;

    for(quint8 p = 0; p < artnet_universe_count; p++)
    {
        for(const auto& nodes : d->_subscribedNodes.values(p))
            d->_socket->writeDatagram(d->artnetDmx(p, 512), nodes.address, artnet_port);
    }
}

void QArtnetDevice::sendPollReply()
{
    Q_D(QArtnetDevice);

    if(!isStarted())
        return;

    qDebug() << "[artnet] Sending poll reply";
    d->_socket->writeDatagram(d->artnetPollReply(), d->_broadcast, artnet_port);
}

void QArtnetDevice::checkSubscriptions()
{
    Q_D(QArtnetDevice);
    for(auto it = d->_subscribedNodes.begin(); it != d->_subscribedNodes.end();)
    {
        if(it->updateTimer.elapsed() >= 10000)
            it = d->_subscribedNodes.erase(it);
        else
            ++it;
    }
}

bool QArtnetDevice::startHook()
{
    Q_D(QArtnetDevice);
    if(d->_socket)
    {
        qWarning() << "[artnet]" << name() << "already have a socket.";
        return false;
    }

    d->_socket = new QUdpSocket(this);

    connect(d->_socket, &QAbstractSocket::errorOccurred,
            this, &QArtnetDevice::socketError);

    connect(d->_socket, &QUdpSocket::readyRead,
            this, &QArtnetDevice::readDatagram);

    if(d->_socket->bind(QHostAddress::AnyIPv4, artnet_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    {
        d->_socket->writeDatagram(d->artnetPoll(), d->_broadcast, artnet_port);
        d->_dmxTimer.start();
        d->_subscriberTimer.start();
        return QDmxGenericNetworkDevice::startHook();
    }
    else
    {
        qWarning() << "[artnet] Unable to bind the socket for" << name();
        return false;
    }
}

bool QArtnetDevice::stopHook()
{
    Q_D(QArtnetDevice);
    if(!d->_socket)
    {
        qWarning() << "[artnet]" << name() << "don't have any socket.";
        return false;
    }

    delete d->_socket;
    d->_socket = nullptr;

    d->_dmxTimer.stop();
    d->_subscriberTimer.stop();

    return QDmxGenericNetworkDevice::stopHook();
}
