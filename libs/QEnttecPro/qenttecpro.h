#pragma once
#include <QtCore>
#include <QDebug>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


#include "qenttecprodefines.h"

class QEnttecPro :
        public QObject
{
    Q_OBJECT

public:

    static QString getVersionString();
    explicit QEnttecPro(int numberOfChannels = 24,
                        int updateIntervalMs = 100,
                        QObject *parent = 0);
    virtual ~QEnttecPro();
	
	// connect to the serial port. valid number of channels is 24-512. performance
	// is directly related to the number of channels, so use the minimum required.

	
	void setLevel(unsigned int channel, unsigned char level);
	void clear();
	unsigned char getLevel(unsigned int channel);	
	
    void setNumberOfChannels(unsigned int channels = 24); // change the number of channels
	bool isConnected();
	
private:

    QTimer *_timer;
    bool _connectedB;
    QByteArray _levels;
    QVector<unsigned char> levels;
    QSerialPort _serial;

    bool _needsUpdate;
	
    bool isValidChannel(unsigned int channel);

public slots:

    bool connect(QString port);
    void disconnect();
    void update(bool force = false); // send a packet to the dmx controller
    void setUpdateInterval(int updateIntervalMs);

signals:

    void connected(QString port);
    void disconnected(QString port);
};
