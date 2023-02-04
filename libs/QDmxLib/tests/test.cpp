#include <QtTest>
#include <QCoreApplication>

#include <qdmxlib/QDmxManager>
#include <qdmxlib/QDmxDriver>
#include <qdmxlib/QDmxDevice>

class TestDmx : public QObject
{
    Q_OBJECT

public:
    TestDmx() = default;

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

void TestDmx::initTestCase()
{
    QVERIFY(QDmxManager::instance()->init());
    QVERIFY(QDmxManager::instance()->isInitialised());
    QVERIFY(!QDmxManager::instance()->configEnabled());
}

void TestDmx::cleanupTestCase()
{
    QVERIFY(QDmxManager::instance()->teardown());
}

void TestDmx::listingDrivers()
{
    for(auto d : QDmxManager::instance()->availableDrivers())
    {
        QVERIFY(d->isLoaded());
        QVERIFY(!d->isEnabled());

        qDebug() <<  d->name();

        d->setEnabled(true);
        QVERIFY(d->isEnabled());
    }
}

void TestDmx::listingDevices()
{
    for(auto d : QDmxManager::instance()->availableDevices())
    {
        QVERIFY(!d->isStarted());
        qDebug() <<  d->name();
    }
}

void TestDmx::driverByName()
{
    QVERIFY(QDmxManager::instance()->driver("dummy"));
    QVERIFY(QDmxManager::instance()->driver("artnet"));
    QVERIFY(!QDmxManager::instance()->driver("__fake__"));
}

void TestDmx::deviceByName()
{
    QVERIFY(QDmxManager::instance()->device("dummy", "dummy"));
    QVERIFY(!QDmxManager::instance()->device("dummy", "__fake__"));
}

void TestDmx::patch()
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

void TestDmx::dmx()
{
    // dummy is a loopback device, it will parot whatever you write back to its
    // input.

    auto* m = QDmxManager::instance();
    QVERIFY(m->patch("dummy", "dummy", 0, 0));

    auto input = m->readInputData(0);
    QCOMPARE(input, QByteArray(512, 0x00));

    auto output = m->readOutputData(0);
    QCOMPARE(output, QByteArray(512, 0x00));

    QSignalSpy spyIn(m, SIGNAL(inputDataChanged(quint8,QByteArray)));
    QSignalSpy spyOut(m, SIGNAL(outputDataChanged(quint8,QByteArray)));

    QVERIFY(m->writeData(0, 0, 100));
    output = m->readOutputData(0);
    auto test = QByteArray(512, 0x00);
    test[0] = 100;
    QCOMPARE(output, test);

    if(spyIn.isEmpty())
        spyIn.wait();

    if(spyOut.isEmpty())
        spyOut.wait();

    QCOMPARE(spyIn.count(), 1);
    QCOMPARE(spyOut.count(), 1);

    QCOMPARE(m->readOutputData(0), m->readInputData(0));

    auto listIn = spyIn.takeFirst();
    auto listOut = spyOut.takeFirst();

    QCOMPARE(listIn, listOut);

    QVERIFY(m->writeData(0, 0, QByteArray(3, 200u)));
    output = m->readOutputData(0);
    test[0] = 200u;
    test[1] = 200u;
    test[2] = 200u;
    QCOMPARE(output, test);

    if(spyIn.isEmpty())
        spyIn.wait();

    if(spyOut.isEmpty())
        spyOut.wait();

    QCOMPARE(spyIn.count(), 1);
    QCOMPARE(spyOut.count(), 1);

    QCOMPARE(m->readOutputData(0), m->readInputData(0));

    listIn = spyIn.takeFirst();
    listOut = spyOut.takeFirst();

    QCOMPARE(listIn, listOut);

    QVERIFY(m->unpatch((quint8)0));
}

QTEST_MAIN(TestDmx)

#include "test.moc"
