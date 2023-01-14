#ifndef QDMXMANAGER_H
#define QDMXMANAGER_H

#include <QObject>
#include <QList>

#include "qdmxlib_global.h"
#include "qdmxdriver.h"
#include "qdmxdevice.h"

class QDmxManagerPrivate;
class QDMXLIB_EXPORT QDmxManager : public QObject
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QDmxManager);

public:
    enum MergeType
    {
        HTP,
        LTP,
        LoTP
    };
    Q_ENUM(MergeType)

    enum PortType
    {
        Input,
        Output
    };
    Q_ENUM(PortType)

    struct PatchInfo
    {
        quint8 universe;
        QDmxDevice* device;
        quint8 port;
    };

protected:
    QDmxManager(QObject* parent = nullptr);

public:
    ~QDmxManager() override;

    static QDmxManager* instance();

    bool init(const QString& configLocation = "./");
    bool teardown();

    QString configLocation();

    QList<QDmxDriver*> availableDrivers() const;
    QList<QDmxDevice*> availableDevices() const;

    template<class T = QDmxDriver>
    T* driver(const QString& name) const
    {
        auto drivers = availableDrivers();
        auto it = std::find_if(drivers.begin(), drivers.end(),
                               [&name](QDmxDriver* dr){ return dr->name() == name; });

        return it == drivers.end() ? nullptr :
                                     dynamic_cast<T*>(*it);
    }

    template<class T = QDmxDevice>
    T* device(const QString& driver, const QString& name) const
    {
        auto dr = this->driver(driver);

        return dr ? dynamic_cast<T*>(dr->device(name))
                  : nullptr;
    }

    bool patch(QDmxDevice* device, quint8 port, quint8 universe, MergeType type = LTP);
    bool patch(const QString& driver, const QString& name, quint8 port, quint8 universe, MergeType type = LTP);

    bool patch(PortType portType, QDmxDevice* device, quint8 port, quint8 universe, MergeType type = LTP);
    bool patch(PortType portType, const QString& driver, const QString& name, quint8 port, quint8 universe, MergeType type = LTP);

    void setMergeType(quint8 universe, MergeType type);

    bool isPatched(quint8 universe);
    bool isPatched(QDmxDevice* device, quint8 port);
    bool isPatched(QDmxDevice* device);
    bool isPatched(const QString& driver, const QString& name, quint8 port);
    bool isPatched(const QString& driver, const QString& name);
    bool isPatched(const QString& driver);
    bool isPatched(QDmxDriver* driver);

    bool isPatched(PortType portType, quint8 universe);
    bool isPatched(PortType portType, QDmxDevice* device, quint8 port);
    bool isPatched(PortType portType, QDmxDevice* device);
    bool isPatched(PortType portType, const QString& driver, const QString& name, quint8 port);
    bool isPatched(PortType portType, const QString& driver, const QString& name);
    bool isPatched(PortType portType, const QString& driver);
    bool isPatched(PortType portType, QDmxDriver* driver);

    bool patchedUniverse(PortType portType, QDmxDevice* device, quint8 port, quint8& universe);
    bool patchedUniverse(PortType portType, const QString& driver, const QString& name, quint8 port, quint8& universe);

    bool unpatch(quint8 universe);
    bool unpatch(QDmxDevice* device, quint8 port);
    bool unpatch(QDmxDevice* device);
    bool unpatch(const QString& driver, const QString& name, quint8 port);
    bool unpatch(const QString& driver, const QString& name);
    bool unpatch(const QString& driver);
    bool unpatch(QDmxDriver* driver);

    bool unpatch(PortType portType, quint8 universe);
    bool unpatch(PortType portType, QDmxDevice* device, quint8 port);
    bool unpatch(PortType portType, QDmxDevice* device);
    bool unpatch(PortType portType, const QString& driver, const QString& name, quint8 port);
    bool unpatch(PortType portType, const QString& driver, const QString& name);
    bool unpatch(PortType portType, const QString& driver);
    bool unpatch(PortType portType, QDmxDriver* driver);

    QByteArray readInputData(quint8 universe);
    QByteArray readOutputData(quint8 universe);

    bool writeData(quint8 universe, quint16 channel, quint8 data);
    bool writeData(quint8 universe, quint16 channel, const QByteArray& data);
    bool writeData(quint8 universe, const QByteArray& data);

protected slots:
    void deviceInputDataChanged(quint8 port, const QByteArray& data);

signals:
    void patchChanged(QDmxManager::PortType portType);
    void availableDevicesChanged();
    void inputDataChanged(quint8 universe, const QByteArray& data);
    void outputDataChanged(quint8 universe, const QByteArray& data);

protected:
    std::unique_ptr<QDmxManagerPrivate> d_ptr;
};

#endif // QDMXMANAGER_H
