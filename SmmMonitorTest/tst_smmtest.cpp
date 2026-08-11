#include <QTest>

#include "../SmmMonitor/smmmanager.h"
#include <utility>

// add necessary includes here

class SmmTest : public QObject
{
    Q_OBJECT

public:
    SmmTest();
    ~SmmTest() override;

private slots:
    void initTestCase();
    void init();
    void cleanupTestCase();
    void cleanup();
    void testParseValidAA55Packet();

private:
    static QByteArray buildPacket(quint8 code, const QByteArray &data);
};

SmmTest::SmmTest() {}

SmmTest::~SmmTest() = default;

void SmmTest::initTestCase()
{
    // code to be executed before the first test function
}

void SmmTest::init()
{
    // code to be executed before each test function
}

void SmmTest::cleanupTestCase()
{
    // code to be executed after the last test function
}

void SmmTest::cleanup()
{
    // code to be executed after each test function
}

QByteArray SmmTest::buildPacket(quint8 code, const QByteArray &data) {
    QByteArray packet;

    packet.append(static_cast<char>(0xAA));
    packet.append(static_cast<char>(0x55));


    const quint8 len = static_cast<quint8>(data.size() + 1);
    packet.append(static_cast<char>(len));
    packet.append(static_cast<char>(code));

    packet.append(data);

    quint32 sum = len + code;
    for(char b : std::as_const(data)) {
        sum += static_cast<quint8>(b);
    }

    const quint8 checksum =  static_cast<quint8>(sum & 0xFF);
    packet.append(static_cast<char>(checksum));

    return packet;
}

void SmmTest::testParseValidAA55Packet() {
    SmmManager manager;
    const int expectedSpo2 = 98;
    const int expectedPulse = 75;
    const quint8 BIOLIGHT_CODE = 21;

    QByteArray data;
    data.resize(9);
    data.fill(0x00);

    data[0] = 0x00; //durum baytı
    data[1] = 50; //waveform
    data[2] = 0x00;
    data[3] = static_cast<char>(expectedSpo2); //spo2
    data[4] = static_cast<char>((expectedPulse >> 8) & 0xFF); //pulse msb
    data[5] = static_cast<char>(expectedPulse & 0xFF); //pulse lsb


    const QByteArray packet = buildPacket(BIOLIGHT_CODE, data);

    manager.injectRawDataForTest(packet);

    QCOMPARE(manager.saturation(), expectedSpo2);
    QCOMPARE(manager.pulseRate(), expectedPulse);

}

QTEST_MAIN(SmmTest)

#include "tst_smmtest.moc"
