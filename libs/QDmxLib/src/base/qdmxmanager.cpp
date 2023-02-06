#include <qdmxlib/private/qdmxmanager.h>

#include <QCoreApplication>
#include <QMetaEnum>

#include <qdmxlib/private/qdmxdriver.h>
#include <qdmxlib/private/qdmxdevice.h>
#include <qdmxlib/private/qartnetdriver.h>
#include <qdmxlib/private/qe131driver.h>
#include <qdmxlib/private/qdmxusbdriver.h>
#include <qdmxlib/private/quartdriver.h>

class QDmxManagerPrivate
{
    Q_DECLARE_PUBLIC(QDmxManager)

    using Patch = QMultiMap<quint8, QDmxManager::PatchInfo>;
    using PatchIterator = Patch::iterator;

public:
    QDmxManagerPrivate() = default;

    void createDrivers();

    PatchIterator findPatchInfo(const Patch& patch, QDmxDevice* device, quint8 port);
    QList<PatchIterator> findPatchInfo(const Patch& patch, QDmxDevice* device);
    bool hasPatch(const Patch& patch, QDmxDevice* device, quint8 port);
    bool hasPatch(const Patch& patch, QDmxDevice* device);

    void merge(QByteArray& dst, const QByteArray& src, QDmxManager::MergeType type);

    QDmxManager* q_ptr = nullptr;

    bool    _initialised = false;
    bool    _configEnabled = false;
    QString _configLocation;
    QList<QDmxDriver*> _allDrivers;

    Patch _inputPatch;
    Patch _outputPatch;
    QMap<quint8, QDmxManager::MergeType> _mergeType;
    QMap<quint8, QByteArray> _inputData;
    QMap<quint8, QByteArray> _outputData;
};

void QDmxManagerPrivate::createDrivers()
{
    Q_Q(QDmxManager);
    _allDrivers << new QDmxDummyDriver(q);
    _allDrivers << new QArtnetDriver(q);
    _allDrivers << new QE131Driver(q);
    _allDrivers << new QDmxUsbDriver(q);
    _allDrivers << new QUartDriver(q);

    for(auto dr : _allDrivers)
    {
        QObject::connect(dr, &QDmxDriver::availableDevicesChanged,
                         q,  &QDmxManager::availableDevicesChanged);
    }
}

QDmxManagerPrivate::PatchIterator QDmxManagerPrivate::findPatchInfo(const Patch& patch,
                                                                    QDmxDevice* device,
                                                                    quint8 port)
{
    auto& rpatch = const_cast<Patch&>(patch);
    return std::find_if(rpatch.begin(), rpatch.end(),
                        [device, port](auto& patch){ return patch.device == device && patch.port == port; });
}

QList<QDmxManagerPrivate::PatchIterator> QDmxManagerPrivate::findPatchInfo(const Patch& patch,
                                                                           QDmxDevice* device)
{
    auto& rpatch = const_cast<Patch&>(patch);

    QList<PatchIterator> its;

    for(auto it = rpatch.begin(); it != rpatch.end(); ++it)
    {
        if(it->device == device)
            its << it;
    }

    return its;
}

bool QDmxManagerPrivate::hasPatch(const Patch& patch, QDmxDevice* device, quint8 port)
{
    auto& rpatch = const_cast<Patch&>(patch);
    return findPatchInfo(patch, device, port) != rpatch.end();
}

bool QDmxManagerPrivate::hasPatch(const Patch& patch, QDmxDevice* device)
{
    return !findPatchInfo(patch, device).isEmpty();
}

void QDmxManagerPrivate::merge(QByteArray& dst, const QByteArray& src, QDmxManager::MergeType type)
{
    Q_ASSERT(dst.size() == 512);
    Q_ASSERT(src.size() == 512);

    switch(type)
    {
    case QDmxManager::HTP:
    {
        auto dit = dst.begin();
        auto sit = src.begin();

        while(dit != dst.end() && sit != src.end())
        {
            if(*sit > *dit)
                *dit = *sit;

            ++dit;
            ++sit;
        }
        break;
    }

    case QDmxManager::LTP:
        dst = src;
        break;

    case QDmxManager::LoTP:
    {
        auto dit = dst.begin();
        auto sit = src.begin();

        while(dit != dst.end() && sit != src.end())
        {
            if(*sit < *dit)
                *dit = *sit;

            ++dit;
            ++sit;
        }
        break;
    }
    }
}

QDmxManager::QDmxManager(QObject* parent) :
    QObject(parent),
    d_ptr(new QDmxManagerPrivate())
{
    Q_D(QDmxManager);
    d->q_ptr = this;
    d->createDrivers();
}

QDmxManager::~QDmxManager()
{
    teardown();
}

QDmxManager* QDmxManager::instance()
{
    static QDmxManager inst;
    return &inst;
}

bool QDmxManager::init()
{
    Q_D(QDmxManager);

    bool success = true;
    for(auto d : qAsConst(d->_allDrivers))
        success &= d->load();

    d->_initialised = success;

    return success;
}

bool QDmxManager::isInitialised() const
{
    return d_func()->_initialised;
}

bool QDmxManager::teardown()
{
    Q_D(QDmxManager);

    qDebug() << "[qdmxlib] teardown.";

    bool success = true;

    auto inputUniverse = d->_inputPatch.keys();
    for(quint8 u : qAsConst(inputUniverse))
        success &= unpatch(Input, u);

    auto outputUniverse = d->_outputPatch.keys();
    for(quint8 u : qAsConst(outputUniverse))
        success &= unpatch(Output, u);

    for(auto d : qAsConst(d->_allDrivers))
    {
        if(d->isLoaded())
            success &= d->unload();
    }

    d->_initialised = false;

    return success;
}

void QDmxManager::setConfigEnabled(bool en)
{
    Q_D(QDmxManager);

    if(d->_initialised)
    {
        qWarning() << "[qdmxlib] Manager is already initialised. No effect.";
        return;
    }

    if(en != d->_configEnabled)
    {
        d->_configEnabled = en;

        if(en)
        {
            if(d->_configLocation.isEmpty())
            {
                qWarning() << "[qdmxlib] No config location set, using the default \"./\"";
                d->_configLocation = "./";
            }
            else
                qDebug() << "[qdmxlib] Config files enabled using location:" << d->_configLocation;
        }
    }
}

bool QDmxManager::configEnabled() const
{
    return d_func()->_configEnabled;
}

void QDmxManager::setConfigLocation(const QString& configLocation)
{
    Q_D(QDmxManager);
    if(d->_initialised)
    {
        qWarning() << "[qdmxlib] Manager is already initialised. No effect.";
        return;
    }

    d->_configLocation = configLocation;
    qDebug() << "[qdmxlib] Config file location changed to:" << configLocation;
}

QString QDmxManager::configLocation() const
{
    return d_func()->_configLocation;
}

QList<QDmxDriver*> QDmxManager::availableDrivers() const
{
    return d_func()->_allDrivers;
}

QList<QDmxDevice*> QDmxManager::availableDevices() const
{
    QList<QDmxDevice*> r;

    auto drivers = availableDrivers();
    for(auto* d : qAsConst(drivers))
        r << d->availableDevices();

    return r;
}

bool QDmxManager::patch(QDmxDevice* device, quint8 port, quint8 universe, MergeType type)
{
    int success = 0;
    if(port < device->inputCount())
    {
        if(patch(Input, device, port, universe, type))
            success++;
    }

    if(port < device->outputCount())
    {
        if(patch(Output, device, port, universe, type))
            success++;
    }

    return success > 0;
}

bool QDmxManager::patch(const QString& driver, const QString& name, quint8 port, quint8 universe, MergeType type)
{
    auto d = device(driver, name);
    if(!d)
    {
        qWarning() << "[qdmxlib] No such device called" << name << "within driver" << driver;
        return false;
    }

    return patch(d, port, universe, type);
}

bool QDmxManager::patch(PortType portType, QDmxDevice* device, quint8 port, quint8 universe, MergeType type)
{
    Q_D(QDmxManager);

    quint8 tmp;
    if(patchedUniverse(portType, device, port, tmp))
    {
        qWarning() << "[qdmxlib]" << device->name() << "port" << port << "is already patched on universe" << tmp;
        return false;
    }

    quint8 count = portType == Input ? device->inputCount() : device->outputCount();
    if(port >= count)
    {
        qWarning() << "[qdmxlib] device" << device->name() << "does not have a" << portType << "with id" << port;
        return false;
    }

    if(!device->isStarted() && !device->start())
    {
        qWarning() << "[qdmxlib] Unable to start device" << device->name();
        return false;
    }

    qDebug() << "[qdmxlib] Patching" << device->name() << (portType == Input ? "input" : "output") << "port" << port << "to universe" << universe;

    auto& patch = portType == Input ? d->_inputPatch : d->_outputPatch;
    auto& data = portType == Input ? d->_inputData : d->_outputData;

    if(portType == Input)
    {
        setMergeType(universe, type);
        connect(device, &QDmxDevice::inputDataChanged,
                this, &QDmxManager::deviceInputDataChanged,
                Qt::UniqueConnection);
    }

    if(!patch.contains(universe))
        data.insert(universe, QByteArray(512, 0x00));

    PatchInfo info;
    info.device = device;
    info.port = port;
    info.universe = universe;

    patch.insert(universe, info);

    if(portType == Input)
        d->merge(data[universe], device->readInputData(port), type);
    else
        device->setData(port, data[universe]);

    emit patchChanged(portType);

    return true;
}

bool QDmxManager::patch(PortType portType, const QString& driver, const QString& name, quint8 port, quint8 universe, MergeType type)
{
    auto d = device(driver, name);
    if(!d)
    {
        qWarning() << "[qdmxlib] No such device called" << name << "within driver" << driver;
        return false;
    }

    return patch(portType, d, port, universe, type);
}

void QDmxManager::setMergeType(quint8 universe, MergeType type)
{
    Q_D(QDmxManager);
    if(d->_mergeType.contains(universe) && d->_mergeType[universe] != type)
        qDebug() << "[qdmxlib] Changing merge of universe" << universe << "type from" << d->_mergeType[universe] << "to" << type;

    d->_mergeType[universe] = type;
}

bool QDmxManager::isPatched(quint8 universe)
{
    return isPatched(Input, universe) || isPatched(Output, universe);
}

bool QDmxManager::isPatched(QDmxDevice* device, quint8 port)
{
    return isPatched(Input, device, port) || isPatched(Output, device, port);
}

bool QDmxManager::isPatched(QDmxDevice* device)
{
    return isPatched(Input, device) || isPatched(Output, device);
}

bool QDmxManager::isPatched(const QString& driver, const QString& name, quint8 port)
{
    auto dev = device(driver, name);
    if(!dev)
        return false;

    return isPatched(dev, port);
}

bool QDmxManager::isPatched(const QString& driver, const QString& name)
{
    auto dev = device(driver, name);
    if(!dev)
        return false;

    return isPatched(dev);
}

bool QDmxManager::isPatched(const QString& driver)
{
    auto dr = this->driver(driver);
    if(!dr)
        return false;

    return isPatched(dr);
}

bool QDmxManager::isPatched(QDmxDriver* driver)
{
    auto devs = driver->availableDevices();
    return std::any_of(devs.begin(), devs.end(), [this](auto* dev){ return isPatched(dev); });
}

bool QDmxManager::isPatched(PortType portType, quint8 universe)
{
    Q_D(QDmxManager);
    const auto& patch = portType == Input ? d->_inputPatch : d->_outputPatch;
    return patch.contains(universe);
}

bool QDmxManager::isPatched(PortType portType, QDmxDevice* device, quint8 port)
{
    Q_D(QDmxManager);
    const auto& patch = portType == Input ? d->_inputPatch : d->_outputPatch;
    return d->hasPatch(patch, device, port);
}

bool QDmxManager::isPatched(PortType portType, QDmxDevice* device)
{
    Q_D(QDmxManager);
    const auto& patch = portType == Input ? d->_inputPatch : d->_outputPatch;
    return d->hasPatch(patch, device);
}

bool QDmxManager::isPatched(PortType portType, const QString& driver, const QString& name, quint8 port)
{
    auto dev = device(driver, name);
    if(!dev)
        return false;

    return isPatched(portType, dev, port);
}

bool QDmxManager::isPatched(PortType portType, const QString& driver, const QString& name)
{
    auto dev = device(driver, name);
    if(!dev)
        return false;

    return isPatched(portType, dev);
}

bool QDmxManager::isPatched(PortType portType, const QString& driver)
{
    auto dr = this->driver(driver);
    if(!dr)
        return false;

    return isPatched(portType, dr);
}

bool QDmxManager::isPatched(PortType portType, QDmxDriver* driver)
{
    auto devs = driver->availableDevices();
    return std::any_of(devs.begin(), devs.end(), [portType, this](auto* dev){ return isPatched(portType, dev); });
}

bool QDmxManager::patchedUniverse(PortType portType, QDmxDevice* device, quint8 port, quint8 &universe)
{
    Q_D(QDmxManager);
    const auto& patch = portType == Input ? d->_inputPatch : d->_outputPatch;
    auto it = d->findPatchInfo(patch, device, port);
    if(it == patch.end())
        return false;

    universe = it->universe;
    return true;
}

bool QDmxManager::patchedUniverse(PortType portType, const QString& driver, const QString& name, quint8 port, quint8 &universe)
{
    auto dev = device(driver, name);
    if(!dev)
        return false;

    return patchedUniverse(portType, dev, port, universe);
}

bool QDmxManager::unpatch(quint8 universe)
{
    bool success = false;
    success |= unpatch(Input, universe);
    success |= unpatch(Output, universe);
    return success;
}

bool QDmxManager::unpatch(QDmxDevice* device, quint8 port)
{
    int success = 0;
    if(isPatched(Input, device, port))
    {
        if(unpatch(Input, device, port))
            success++;
    }

    if(isPatched(Output, device, port))
    {
        if(unpatch(Output, device, port))
            success++;
    }

    return success > 0;
}

bool QDmxManager::unpatch(QDmxDevice* device)
{
    int success = 0;
    if(isPatched(Input, device))
    {
        if(unpatch(Input, device))
            success++;
    }

    if(isPatched(Output, device))
    {
        if(unpatch(Output, device))
            success++;
    }

    return success > 0;
}

bool QDmxManager::unpatch(const QString& driver, const QString& name, quint8 port)
{
    auto dev = device(driver, name);
    if(!dev)
    {
        qWarning() << "[qdmxlib] No such device called" << name << "within driver" << driver;
        return false;
    }

    int success = 0;
    if(isPatched(Input, dev, port))
    {
        if(unpatch(Input, dev, port))
            success++;
    }

    if(isPatched(Output, dev, port))
    {
        if(unpatch(Output, dev, port))
            success++;
    }

    return success > 0;
}

bool QDmxManager::unpatch(const QString& driver, const QString& name)
{
    auto dev = device(driver, name);
    if(!dev)
    {
        qWarning() << "[qdmxlib] No such device called" << name << "within driver" << driver;
        return false;
    }

    int success = 0;
    if(isPatched(Input, dev))
    {
        if(unpatch(Input, dev))
            success++;
    }

    if(isPatched(Output, dev))
    {
        if(unpatch(Output, dev))
            success++;
    }

    return success > 0;
}

bool QDmxManager::unpatch(const QString& driver)
{
    auto dr = this->driver(driver);
    if(!dr)
    {
        qWarning() << "[qdmxlib] No such driver called" << driver;
        return false;
    }

    return unpatch(dr);
}

bool QDmxManager::unpatch(QDmxDriver* driver)
{
    int success = 0;
    if(isPatched(Input, driver))
    {
        if(unpatch(Input, driver))
            success++;
    }

    if(isPatched(Output, driver))
    {
        if(unpatch(Output, driver))
            success++;
    }

    return success > 0;
}

bool QDmxManager::unpatch(PortType portType, quint8 universe)
{
    Q_D(QDmxManager);

    auto& patch = portType == Input ? d->_inputPatch : d->_outputPatch;

    QList<QDmxDevice*> devs;

    for(const auto& p : patch.values(universe))
    {
        if(!devs.contains(p.device))
            devs << p.device;
    }

    patch.remove(universe);

    for(auto d : devs)
    {
        if(portType == Input && !isPatched(Input, d))
        {
            disconnect(d, &QDmxDevice::inputDataChanged,
                       this, &QDmxManager::deviceInputDataChanged);
        }

        if(!isPatched(d))
            d->stop();
    }

    emit patchChanged(portType);

    return true;
}

bool QDmxManager::unpatch(PortType portType, QDmxDevice* device, quint8 port)
{
    Q_D(QDmxManager);

    auto& patch = portType == Input ? d->_inputPatch : d->_outputPatch;
    auto& data = portType == Input ? d->_inputData : d->_outputData;

    auto it = d->findPatchInfo(patch, device, port);
    if(it == patch.end())
    {
        qWarning() << "[qdmxlib]" << portType << "Port" << port << "of device" << device->name() << "is not patched.";
        return false;
    }

    auto universe = it->universe;
    patch.erase(it);

    if(portType == Input && !isPatched(Input, device))
    {
        disconnect(device, &QDmxDevice::inputDataChanged,
                   this, &QDmxManager::deviceInputDataChanged);
    }

    if(!isPatched(device))
        device->stop();

    if(!patch.contains(universe))
    {
        data.remove(universe);

        if(portType == Input)
            d->_mergeType.remove(universe);
    }

    emit patchChanged(portType);

    return true;
}

bool QDmxManager::unpatch(PortType portType, QDmxDevice* device)
{
    Q_D(QDmxManager);
    auto& patch = portType == Input ? d->_inputPatch : d->_outputPatch;
    auto& data = portType == Input ? d->_inputData : d->_outputData;

    auto its = d->findPatchInfo(patch, device);

    QList<quint8> universes;
    for(const auto& it : its)
    {
        if(!universes.contains(it->universe))
            universes << it->universe;

        patch.erase(it);
    }

    if(portType == Input && !isPatched(Input, device))
    {
        disconnect(device, &QDmxDevice::inputDataChanged,
                   this, &QDmxManager::deviceInputDataChanged);
    }

    if(!isPatched(device))
        device->stop();



    for(auto universe : universes)
    {
        if(!patch.contains(universe))
        {
            data.remove(universe);

            if(portType == Input)
                d->_mergeType.remove(universe);
        }
    }

    emit patchChanged(portType);

    return true;
}

bool QDmxManager::unpatch(PortType portType, const QString& driver, const QString& name, quint8 port)
{
    auto dev = device(driver, name);
    if(!dev)
    {
        qWarning() << "[qdmxlib] No such device called" << name << "within driver" << driver;
        return false;
    }

    return unpatch(portType, dev, port);
}

bool QDmxManager::unpatch(PortType portType, const QString& driver, const QString& name)
{
    auto dev = device(driver, name);
    if(!dev)
    {
        qWarning() << "[qdmxlib] No such device called" << name << "within driver" << driver;
        return false;
    }

    return unpatch(portType, dev);
}

bool QDmxManager::unpatch(PortType portType, const QString& driver)
{
    auto dr = this->driver(driver);
    if(!dr)
    {
        qWarning() << "[qdmxlib] No such driver called" << driver;
        return false;
    }

    return unpatch(portType, dr);
}

bool QDmxManager::unpatch(PortType portType, QDmxDriver* driver)
{
    bool success = true;

    for(auto dev : driver->availableDevices())
    {
        if(isPatched(portType, dev))
            success &= unpatch(portType, dev);
    }

    return success;
}

QByteArray QDmxManager::readInputData(quint8 universe)
{
    return d_func()->_inputData.value(universe);
}

QByteArray QDmxManager::readOutputData(quint8 universe)
{
    return d_func()->_outputData.value(universe);
}

bool QDmxManager::writeData(quint8 universe, quint16 channel, quint8 data)
{
    Q_D(QDmxManager);
    Q_ASSERT(channel < 512);

    if(!d->_outputData.contains(universe))
    {
        qWarning() << "[qdmxlib] universe" << universe << "is not patched";
        return false;
    }

    d->_outputData[universe][channel] = data;
    for(const auto& p : d->_outputPatch.values(universe))
        p.device->setData(p.port, channel, data);

    emit outputDataChanged(universe, d->_outputData[universe]);
    return true;
}

bool QDmxManager::writeData(quint8 universe, quint16 channel, const QByteArray& data)
{
    Q_D(QDmxManager);
    Q_ASSERT(channel + data.size() <= 512);

    if(!d->_outputData.contains(universe))
    {
        qWarning() << "[qdmxlib] universe" << universe << "is not patched";
        return false;
    }

    d->_outputData[universe].replace(channel, data.size(), data);
    for(const auto& p : d->_outputPatch.values(universe))
        p.device->setData(p.port, channel, data);

    emit outputDataChanged(universe, d->_outputData[universe]);
    return true;
}

bool QDmxManager::writeData(quint8 universe, const QByteArray& data)
{
    return writeData(universe, 0, data);
}

void QDmxManager::deviceInputDataChanged(quint8 port, const QByteArray& data)
{
    Q_D(QDmxManager);

    auto dev = dynamic_cast<QDmxDevice*>(sender());
    if(!dev)
        return;

    auto it = d->findPatchInfo(d->_inputPatch, dev, port);
    if(it == d->_inputPatch.end())
        return;

    QByteArray& dest = d->_inputData[it->universe];
    QByteArray copy = dest;

    d->merge(dest, data, d->_mergeType[it->universe]);

    if(dest != copy)
        emit inputDataChanged(it->universe, dest);
}
