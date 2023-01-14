#ifndef QDMXUDMXPLUGIN_H
#define QDMXUDMXPLUGIN_H

#include <QDmxIO.h>
#include "QDmxUDmxDevice.h"

class QDmxUDmxPlugin : public QDmxIO
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDmxPlugin_iid)
    Q_INTERFACES(QDmxIO)

public:
    QDmxUDmxPlugin(QObject* parent = 0);
    virtual ~QDmxUDmxPlugin();

    //===== GENERAL
    virtual void init();
    virtual QString name() { return "uDmx"; }

    //===== DEVICE
    virtual QStringList getDevices();
    virtual bool deviceExists(int device)           { return device >= 0 && device < _deviceList.length(); }

    //===== INPUTS
    virtual int  getInputCount(int device)          { Q_UNUSED(device) return 0; }
    virtual bool inputExists(int device, int port)  { Q_UNUSED(device) Q_UNUSED(port) return false; }
    virtual bool openInput(int device, int port)    { Q_UNUSED(device) Q_UNUSED(port) return false; }
    virtual bool closeInput(int device, int port)   { Q_UNUSED(device) Q_UNUSED(port) return false; }

    //===== OUTPUTS
public:
    virtual int  getOutputCount(int device)         { if(deviceExists(device)) return 1; return 0; }
    virtual bool outputExists(int device, int port) { return deviceExists(device) && port == 0; }
    virtual bool openOutput(int device, int port);
    virtual bool closeOutput(int device, int port);

public slots:
    virtual void writeDmx(int device, int port, QByteArray data);

private:
    void refreshDevices();
    QDmxUDmxDevice* device(struct usb_device* dev);

private:
    QList<QDmxUDmxDevice*> _deviceList;
};
#endif // QDMXUDMXPLUGIN_H
