#include <QtTest>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QCoreApplication>

#include <qdmxlib/QDmxManager>
#include <qdmxlib/QDmxDriver>
#include <qdmxlib/QDmxDevice>
#include <qdmxlib/QArtnetDevice>

class TestArtnet : public QObject
{
    Q_OBJECT

public:
    TestArtnet() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test();

private:
    QDmxManager* m;
    QArtnetDevice* d;
    QUdpSocket s;
};

void TestArtnet::initTestCase()
{
    QFileInfo fi(QFINDTESTDATA("configs/artnet.json"));

    m = QDmxManager::instance();
    QVERIFY(m->init(fi.absolutePath()));

    d = m->device<QArtnetDevice>("artnet", "en7");

    QVERIFY(d);
    QVERIFY(m->patch(d, 0, 0));
}

void TestArtnet::cleanupTestCase()
{
    QVERIFY(QDmxManager::instance()->teardown());
}

void TestArtnet::test()
{
    QSignalSpy spy(m, SIGNAL(inputDataChanged(quint8,QByteArray)));

    for(quint8 i = 0; i < 10; i++)
    {
        qDebug() << "Sending" << i*10;
        QVERIFY(m->writeData(0, 0, i*10));
        auto data = m->readOutputData(0);
        QCOMPARE(data[0], (char)(i*10));
        QTest::qWait(50);
    }

    spy.wait();

    QVERIFY(spy.count() > 0);

    auto args = spy.at(0);

    QCOMPARE(args[0].toInt(), 0);
    auto data = args[1].toByteArray();
    QVERIFY(data[0] != '\x00');
}

QTEST_MAIN(TestArtnet)

#include "tst_artnet.moc"
