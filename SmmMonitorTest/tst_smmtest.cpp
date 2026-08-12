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

    void testInvalidChecksumPacket();
    void testIncompletePacket();
    void testNoisyHeaderPacket();

    void testSpo2AlarmTrigger();
    void testBitmaskStates();

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

void SmmTest::testInvalidChecksumPacket() {
    SmmManager manager;
    QByteArray data;
    data.resize(9);
    data.fill(0x00);
    data[3] = 98;
    data[4] = 0;
    data[5] = 75;

    QByteArray packet = buildPacket(21, data);

    // Son baytı güvenli bir şekilde al ve 1 artırarak boz
    char corruptedChecksum = static_cast<char>(packet.at(packet.size() - 1) + 1);
    packet[packet.size() - 1] = corruptedChecksum;

    manager.injectRawDataForTest(packet);

    QCOMPARE(manager.saturation(), 0);
    QCOMPARE(manager.pulseRate(), 0);


}

void SmmTest::testIncompletePacket() {
    SmmManager manager ;

    QByteArray data;
    data.resize(9);
    data.fill(0x00);
    data[3] = 97;
    data[5] = 80;

    QByteArray packet = buildPacket(21, data);

    packet.chop(3);

    manager.injectRawDataForTest(packet);
    QCOMPARE(manager.saturation(), 0);
    QCOMPARE(manager.pulseRate(), 0);

}

void SmmTest::testNoisyHeaderPacket() {
    SmmManager manager;
    const int expectedSpo2 = 99;
    const int expectedPulse = 72;

    QByteArray data;
    data.resize(9);
    data.fill(0x00);
    data[3] = static_cast<char>(expectedSpo2);
    data[4] = static_cast<char>((expectedPulse >> 8) & 0xFF);
    data[5] = static_cast<char>(expectedPulse & 0xFF);

    QByteArray validPacket = buildPacket(21, data);

    QByteArray noisyPacket;

    noisyPacket.append(static_cast<char>(0xFF));
    noisyPacket.append(static_cast<char>(0x12));
    noisyPacket.append(static_cast<char>(0x00));
    noisyPacket.append(validPacket);

    manager.injectRawDataForTest(noisyPacket);
    QCOMPARE(manager.saturation(), expectedSpo2);
    QCOMPARE(manager.pulseRate(), expectedPulse);

}

void SmmTest::testSpo2AlarmTrigger() {
    SmmManager manager;

    // 1. Önce portu bağlı hale getir ki alarm ve veri işleme aktif olsun
    manager.connectToModule("COM_TEST"); // Veya simüle edilmiş port durumu
    // Alternatif olarak port bağlı bayrağını tetikleyecek uygun bir başlangıç yapılabilir.

    manager.setSpo2LowerLimit(90);

    QByteArray payload(9, 0);
    payload[3] = 85; // Alt limitin altında SpO2 değeri (85 < 90)

    QByteArray packet = buildPacket(21, payload); // Biolight code = 21
    manager.injectRawDataForTest(packet);

    QVERIFY(manager.isSpo2AlarmActive() == true);
}

void SmmTest::testBitmaskStates() {
    SmmManager manager;
    QByteArray payload1(9, 0);

    // isWeak kontrolü (data0 & (1 << 4)) -> 0x10 maskesi gerektirir
    payload1[0] = 0x10;

    QByteArray packet1 = buildPacket(21, payload1);
    manager.injectRawDataForTest(packet1);

    QVERIFY(manager.isSignalWeak() == true);
}

QTEST_MAIN(SmmTest)

#include "tst_smmtest.moc"

