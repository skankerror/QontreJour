#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include <QDmxIO.h>

class TestESPNet : public QObject
{
    Q_OBJECT

public:
    TestESPNet();

private Q_SLOTS:
    void initTestCase();

    void testLoad();
    void testInit();
    void testName();
    void testDevice();

    void testInputCountAndExist();
    void testOpenInput();
    void testDmxInput();
    void closeInput();

    void testOutputCountAndExist();
    void testOpenOutput();
    void testDmxOutput();
    void closeOutput();

    void cleanupTestCase();


private:
    QDmxIO* plug;
};

TestESPNet::TestESPNet()
{
    plug = 0;
}

void TestESPNet::initTestCase()
{
    QCoreApplication::addLibraryPath("/Users/nicolas/Programmation/plugin/");
}

void TestESPNet::testLoad()
{
    QPluginLoader loader("libQDmxESPNet");
    QVERIFY(loader.load());
    plug = qobject_cast<QDmxIO*>(loader.instance());
    QVERIFY(plug);
}

void TestESPNet::testInit()
{
    if(plug)
    {
        plug->init();
        QString error = plug->errorString();
        QVERIFY2(error.isEmpty(), "init error");
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::testName()
{
    if(plug)
        QCOMPARE(plug->name(), QString("ESP Net"));
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::testDevice()
{
    if(plug)
    {
        QVERIFY2(plug->getDevices().size() > 0, "there is no device");
        QVERIFY2(plug->deviceExists(0), "the ethernet interface is not connected");
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::testInputCountAndExist()
{
    if(plug)
    {
        QVERIFY(plug->getInputCount(0) == 255);
        QVERIFY(plug->inputExists(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::testOpenInput()
{
    if(plug)
    {
        QVERIFY(plug->openInput(0, 0));
        QVERIFY(plug->inputIsOpened(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::testDmxInput()
{
    if(plug)
    {
        QSignalSpy spy(plug, SIGNAL(dmxInput(quint32,quint32,QByteArray)));
        QVERIFY(spy.wait());

        QList<QVariant> args = spy.takeFirst();
        QVERIFY(args.at(0).type() == QVariant::UInt);
        QVERIFY(args.at(1).type() == QVariant::UInt);
        QVERIFY(args.at(2).type() == QVariant::ByteArray);
        qDebug() << args.at(0).toInt() << args.at(1).toInt();
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::closeInput()
{
    if(plug)
    {
        if(!plug->closeInput(0, 0))
        {
            qDebug() << plug->errorString();
            QFAIL("failed");
        }
        QVERIFY(!plug->inputIsOpened(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::testOutputCountAndExist()
{
    if(plug)
    {
        QVERIFY(plug->getOutputCount(0) == 255);
        QVERIFY(plug->outputExists(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::testOpenOutput()
{
    if(plug)
    {
        QVERIFY(plug->openOutput(0, 0));
        QVERIFY(plug->outputIsOpened(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::testDmxOutput()
{
    if(plug)
    {
        qDebug() << "output...";
        for(int i = 0; i < 10; i++)
        {
            QByteArray data;
            data.resize(512);

            qDebug() << i;
            plug->writeDmx(0, 0, data);
            QTest::qWait(500);
        }
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::closeOutput()
{
    if(plug)
    {
        if(!plug->closeOutput(0, 0))
        {
            qDebug() << plug->errorString();
            QSKIP("failed");
        }
        QVERIFY(!plug->outputIsOpened(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestESPNet::cleanupTestCase()
{
}



QTEST_MAIN(TestESPNet)

#include "tst_TestESPNet.moc"
