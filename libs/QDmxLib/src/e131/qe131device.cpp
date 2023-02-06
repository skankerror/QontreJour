#include <qdmxlib/private/qe131device.h>
#include <qdmxlib/private/qe131driver.h>
#include <qdmxlib/private/qdmxgenericnetworkdevice_p.h>

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <array>

constexpr const int e131_universe_count = 4;
constexpr const int e131_port           = 5568;

class QE131DevicePrivate : public QDmxGenericNetworkDevicePrivate
{
    Q_DECLARE_PUBLIC(QE131Device);

    Q_DECLARE_DMX_DRIVER(QE131Driver);

public:
    QE131DevicePrivate(const QNetworkInterface& interface,
                         const QNetworkAddressEntry& entry,
                         QDmxDriver* driver);

    QByteArray e131Dmx(quint8 universe, quint16 length = 512, quint8 priority = 108);

    bool extractDmxData(const QByteArray& data, quint8& port);

    QUdpSocket* _socket = nullptr;

    std::array<quint8, e131_universe_count> _inputSequence;
    std::array<quint8, e131_universe_count> _outputSequence;
};

QE131DevicePrivate::QE131DevicePrivate(const QNetworkInterface& interface,
                                       const QNetworkAddressEntry& entry,
                                       QDmxDriver* driver) :
    QDmxGenericNetworkDevicePrivate(interface, entry, driver)
{
    _inputCount = e131_universe_count;
    _outputCount = e131_universe_count;

    for(auto& c : _inputSequence)
        c = 1;

    for(auto& c : _outputSequence)
        c = 1;
}

QByteArray QE131DevicePrivate::e131Dmx(quint8 port, quint16 length, quint8 priority)
{
    Q_DR(QE131Driver);

    const quint16 totalLength = 126+length; // Alway the safe data size
    const quint16 e131Length  = totalLength - 38;
    const quint16 dmpLength   = totalLength - 115;
    const quint16 dmxLength   = length + 1;

    QByteArray data;

    // Preamble Size
    data.append('\x00');
    data.append('\x10');

    // Post-amble Size
    data.append('\x00');
    data.append('\x00');

    // Identifies this packet as E1.17
    data.append('\x41');
    data.append('\x53');
    data.append('\x43');
    data.append('\x2D');
    data.append('\x45');
    data.append('\x31');
    data.append('\x2E');
    data.append('\x31');
    data.append('\x37');
    data.append('\x00');
    data.append('\x00');
    data.append('\x00');

    // Flags + Root PDU Length
    data.append(0x70 + ((totalLength >> 8) & 0x0F));
    data.append(totalLength & 0xFF);

    // Identifies RLP Data as 1.31 Protocol PDU
    data.append('\x00');
    data.append('\x00');
    data.append('\x00');
    data.append('\x04');

    //Sender's CID
    data.append('\xFB');
    data.append('\x3C');
    data.append('\x10');
    data.append('\x65');
    data.append('\xA1');
    data.append('\x7F');
    data.append('\x4D');
    data.append('\xE2');
    data.append('\x99');
    data.append('\x19');
    data.append('\x31');
    data.append('\x7A');
    data.append('\x07');
    data.append('\xC1');
    data.append('\x00');
    data.append('\x52');

    // Flags + Framing PDU Length
    data.append(0x70 + ((e131Length >> 8) & 0x0F));
    data.append(e131Length & 0xFF);

    // Identifies 1.31 data as DMP Protocol PDU
    data.append('\x00');
    data.append('\x00');
    data.append('\x00');
    data.append('\x02');

    // User Assigned Name of source !!must be 64 bytes long!!
    data.append(dr->sourceName().toUtf8().leftJustified(64, '\x00', true));

    // Data priority if multiple sources (default to 100) (byte 108)
    data.append(priority);

    // Sync Universe
    data.append('\x00');
    data.append('\x00');

    // Sequence
    data.append(_outputSequence[port]);

    // Options Flags - default to none
    data.append('\x00');

    // Universe
    data.append('\x00');
    data.append(port);

    // Protocol flags and length
    data.append(0x70 + ((dmpLength >> 8) & 0x0F));
    data.append(dmpLength & 0xFF);

    // Identifies DMP Set Property Message PDU
    data.append('\x02');

    // Address type & Data type - Identifies format of address and data
    data.append('\xA1');

    // First Property address - Identifies format of address and data
    data.append('\x00');
    data.append('\x00');

    // Address Increment - Indicates each property is 1 octet
    data.append('\x00');
    data.append('\x01');

    // Property value count -  Indicates 1+ the number of slots in packet
    data.append(dmxLength >> 8);
    data.append(dmxLength & 0xFF);

    // DMX512-A START Code
    data.append('\x00');

    data.append(readBuffer(_outputData, port, length));

    if (_outputSequence[port] == 0xFF)
        _outputSequence[port] = 1;
    else
        _outputSequence[port]++;

    return data;
}

bool QE131DevicePrivate::extractDmxData(const QByteArray& data, quint8& port)
{
    if(data.mid(4, 12) != QByteArray("\x41\x53\x43\x2D\x45\x31\x2E\x31\x37\x00\x00\x00", 12))
        return false;

    if(data.mid(40, 4) != QByteArray("\x00\x00\x00\x02", 4))
        return false;

    port = (data[113] << 8) + data[114];

    if(port >= e131_universe_count)
        return false;

    quint8 seq = data[111];

    if(seq < _inputSequence[port])
        return false;

    _inputSequence[port] = seq;

    int length = (data[123] << 8) + data[124];

    auto quint8 = data.mid(126, length);

    updateBuffer(_inputData, port, quint8);

    return true;
}

QE131Device::QE131Device(const QNetworkInterface& interface,
                         const QNetworkAddressEntry& entry,
                         QDmxGenericNetworkDriver* parent) :
    QDmxGenericNetworkDevice(*new QE131DevicePrivate(interface, entry, parent), parent)
{

}

void QE131Device::setData(quint8 port, quint16 channel, quint8 data)
{
    Q_D(QE131Device);
    QDmxGenericNetworkDevice::setData(port, channel, data);
    auto quint8 = d->e131Dmx(port, channel+1);
    QHostAddress addr(0xECFF0000 + (port + 1));
    d->_socket->writeDatagram(quint8, addr, e131_port);
}

void QE131Device::setData(quint8 port, quint16 channel, const QByteArray& data)
{
    Q_D(QE131Device);
    QDmxGenericNetworkDevice::setData(port, channel, data);
    auto quint8 = d->e131Dmx(port, channel+data.size());
    QHostAddress addr(0xECFF0000 + (port + 1));
    d->_socket->writeDatagram(quint8, addr, e131_port);
}

void QE131Device::setData(quint8 port, const QByteArray& data)
{
    setData(port, 0, data);
}

void QE131Device::readDatagram()
{
    Q_D(QE131Device);

    while(d->_socket->hasPendingDatagrams())
    {
        auto datagram = d->_socket->receiveDatagram();
        auto senderAddress = datagram.senderAddress();
        if(senderAddress != d->_address && senderAddress.isInSubnet(d->_address, d->_entry.prefixLength()))
        {
           quint8 u;
           if(d->extractDmxData(datagram.data(), u))
                emit inputDataChanged(u-1, readInputData(u-1));
        }
    }
}

void QE131Device::socketError(QAbstractSocket::SocketError socketError)
{
    qWarning() << "[e131] Socker error:" << socketError;
}

bool QE131Device::startHook()
{
    Q_D(QE131Device);
    if(d->_socket)
    {
        qWarning() << "[e131]" << name() << "already have a socket.";
        return false;
    }

    d->_socket = new QUdpSocket(this);

    connect(d->_socket, &QAbstractSocket::errorOccurred,
            this, &QE131Device::socketError);

    connect(d->_socket, &QUdpSocket::readyRead,
            this, &QE131Device::readDatagram);

    if(d->_socket->bind(QHostAddress::AnyIPv4, e131_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    {
        for(quint8 i = 0; i < e131_universe_count; i++)
        {
            QHostAddress addr(0xECFF0000 + (i + 1));
            if(!d->_socket->joinMulticastGroup(addr, d->_interface))
            {
                qWarning() << "[e131] Unable to join the multicast group" << addr;
                return false;
            }
        }

        return QDmxGenericNetworkDevice::startHook();
    }
    else
    {
        qWarning() << "[e131] Unable to bind the socket for" << name();
        return false;
    }
}

bool QE131Device::stopHook()
{
    Q_D(QE131Device);
    if(!d->_socket)
    {
        qWarning() << "[e131]" << name() << "don't have any socket.";
        return false;
    }

    delete d->_socket;
    d->_socket = nullptr;

    return QDmxGenericNetworkDevice::stopHook();
}
