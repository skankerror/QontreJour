#include <QtTest>
#include <QCoreApplication>

#include <qdmxlib/QDmxManager>
#include <qdmxlib/QDmxDriver>
#include <qdmxlib/QDmxDevice>

class TestManager : public QObject
{
    Q_OBJECT

public:
    TestManager() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void listingDrivers();
    void listingDevices();
    void driverByName();
    void deviceByName();
    void patch();
    void dmx();
};

void TestManager::initTestCase()
{
    QFileInfo fi(QFINDTESTDATA("configs/dummy.json"));
    QVERIFY(QDmxManager::instance()->init(fi.absolutePath()));
}

void TestManager::cleanupTestCase()
{
    QVERIFY(QDmxManager::instance()->teardown());
}

void TestManager::listingDrivers()
{
    for(auto d : QDmxManager::instance()->availableDrivers())
    {
        QVERIFY(d->isEnabled());
        QVERIFY(d->isLoaded());

        qDebug() <<  d->name();
    }
}

void TestManager::listingDevices()
{
    for(auto d : QDmxManager::instance()->availableDevices())
    {
        QVERIFY(!d->isStarted());
        qDebug() <<  d->name();
    }
}

void TestManager::driverByName()
{
    QVERIFY(QDmxManager::instance()->driver("dummy"));
    QVERIFY(QDmxManager::instance()->driver("artnet"));
    QVERIFY(!QDmxManager::instance()->driver("__fake__"));
}

void TestManager::deviceByName()
{
    QVERIFY(QDmxManager::instance()->device("dummy", "dummy"));
    QVERIFY(!QDmxManager::instance()->device("dummy", "__fake__"));
}

void TestManager::patch()
{
    auto* m = QDmxManager::instance();
    QVERIFY(m->patch("dummy", "dummy", 0, 0));
    QVERIFY(m->isPatched("dummy", "dummy", 0));
    QVERIFY(!m->isPatched("dummy", "dummy", 1));
    QVERIFY(m->isPatched("dummy"));
    QVERIFY(!m->isPatched("artnet"));

    quint8 u;
    QVERIFY(m->patchedUniverse(QDmxManager::Input, "dummy", "dummy", 0, u));
    QVERIFY(u == 0);
    QVERIFY(m->patchedUniverse(QDmxManager::Output, "dummy", "dummy", 0, u));
    QVERIFY(u == 0);
    QVERIFY(!m->patchedUniverse(QDmxManager::Input, "dummy", "dummy", 1, u));
    QVERIFY(!m->patchedUniverse(QDmxManager::Output, "dummy", "dummy", 1, u));

    QVERIFY(m->unpatch((quint8)0));
    QVERIFY(!m->isPatched("dummy"));
}

void TestManager::dmx()
{
    auto* m = QDmxManager::instance();
    QVERIFY(m->patch("dummy", "dummy", 0, 0));

    auto input = m->readInputData(0);
    QCOMPARE(input, QByteArray(512, 0x00));

    auto output = m->readOutputData(0);
    QCOMPARE(output, QByteArray(512, 0x00));

    QVERIFY(m->writeData(0, 0, 100));
    output = m->readOutputData(0);
    auto test = QByteArray(512, 0x00);
    test[0] = 100;
    QCOMPARE(output, test);

    QVERIFY(m->writeData(0, 0, QByteArray(3, 200u)));
    output = m->readOutputData(0);
    test[0] = 200u;
    test[1] = 200u;
    test[2] = 200u;
    QCOMPARE(output, test);

    QVERIFY(m->unpatch((quint8)0));
}

QTEST_MAIN(TestManager)

#include "tst_manager.moc"
