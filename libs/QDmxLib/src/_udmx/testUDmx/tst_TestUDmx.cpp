#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include <QDmxIO.h>

class TestUDmx : public QObject
{
    Q_OBJECT

public:
    TestUDmx();

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

TestUDmx::TestUDmx()
{
    plug = 0;
}

void TestUDmx::initTestCase()
{
    QCoreApplication::addLibraryPath("/Users/nicolas/Programmation/plugin/");
}

void TestUDmx::testLoad()
{
    QPluginLoader loader("libQDmxESPNet");
    QVERIFY(loader.load());
    plug = qobject_cast<QDmxIO*>(loader.instance());
    QVERIFY(plug);
}

void TestUDmx::testInit()
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

void TestUDmx::testName()
{
    if(plug)
        QCOMPARE(plug->name(), QString("ESP Net"));
    else
        QSKIP("plug is not loaded");
}

void TestUDmx::testDevice()
{
    if(plug)
    {
        QVERIFY2(plug->getDevices().length() > 0, "there is no device");
        QVERIFY2(plug->deviceExists(0), "the ethernet interface is not connected");
    }
    else
        QSKIP("plug is not loaded");
}

void TestUDmx::testInputCountAndExist()
{
    if(plug)
    {
        QCOMPARE(plug->getInputCount(0), 255);
        QVERIFY(plug->inputExists(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestUDmx::testOpenInput()
{
    if(plug)
    {
        QVERIFY(plug->openInput(0, 0));
        QVERIFY(plug->inputIsOpened(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestUDmx::testDmxInput()
{
    if(plug)
    {
        QSignalSpy spy(plug, SIGNAL(dmxInput(int,int,QByteArray)));
        QVERIFY(spy.wait());

        QList<QVariant> args = spy.takeFirst();
        QVERIFY(args.at(0).type() == QVariant::Int);
        QVERIFY(args.at(1).type() == QVariant::Int);
        QVERIFY(args.at(2).type() == QVariant::ByteArray);
        qDebug() << args.at(0).toInt() << args.at(1).toInt();
    }
    else
        QSKIP("plug is not loaded");
}

void TestUDmx::closeInput()
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

void TestUDmx::testOutputCountAndExist()
{
    if(plug)
    {
        QCOMPARE(plug->getOutputCount(0), 255);
        QVERIFY(plug->outputExists(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestUDmx::testOpenOutput()
{
    if(plug)
    {
        QVERIFY(plug->openOutput(0, 0));
        QVERIFY(plug->outputIsOpened(0, 0));
    }
    else
        QSKIP("plug is not loaded");
}

void TestUDmx::testDmxOutput()
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

void TestUDmx::closeOutput()
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

void TestUDmx::cleanupTestCase()
{
}



QTEST_MAIN(TestESPNet)

#include "tst_TestESPNet.moc"
