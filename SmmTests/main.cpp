#include <QtQuickTest/quicktest.h> //QML test modülü
#include <QGuiApplication> //QML arayüzleri için gerekli uygulama
#include <QtTest>
#include <smmmanager.h>
#include <QRandomGenerator>


class SmmTest : public QObject {
    Q_OBJECT

private slots:
    void test_gecerli_spo2_paketi(){
        SmmManager manager;
        QByteArray mockPacket;

        //Biolight protokolüne uygun sahte paket
        mockPacket.append(static_cast<char>(0xAA)); //header 1
        mockPacket.append(static_cast<char>(0x55)); //header 2
        mockPacket.append(static_cast<char>(10)); // uzunluk(len)
        mockPacket.append(static_cast<char>(21));; //kod(biolight code =0x15 = 21)

        //data kısmı(9 byte)
        mockPacket.append(static_cast<char>(0x00)); //data[0]: inSensorOff
        mockPacket.append(static_cast<char>(0x00)); //data[1]
        mockPacket.append(static_cast<char>(0x00)); //data[2]
        mockPacket.append(static_cast<char>(98)); //data[3]: rawSpO2
        mockPacket.append(static_cast<char>(0x00));  //data[4]
        mockPacket.append(static_cast<char>(0x00)); //data[5]
        mockPacket.append(static_cast<char>(0x00));//data[6]
        mockPacket.append(static_cast<char>(0x00)); //data[7]
        mockPacket.append(static_cast<char>(0x00));//data[8]

        //CheckSum hesaplaması(Len + Code + Data byte'larının toplamı)
        quint32 sum = 10 + 21 + 0 + 0 + 98 + 0 + 0 + 0 + 0 + 0;
        mockPacket.append(static_cast<char>(sum & 0xFF));

        //sahte veriyi modüle injecte et
        manager.injectTestData(mockPacket);

        //beklenen sonuç:modülün ayrıştırıp saturation değerini 98 yapması
        QCOMPARE(manager.saturation(), 98);

    }
    //senaryo 2: Sensör parmaktan çıkarıldığında (sensor off)
    void test_sensor_cikarildi() {
        SmmManager manager;
        QByteArray mockPacket;

        mockPacket.append(static_cast<char>(0xAA));
        mockPacket.append(static_cast<char>(0x55));
        mockPacket.append(static_cast<char>(10));
        mockPacket.append(static_cast<char>(21));

        //0x4o durumu sensör çıktı demektir
        mockPacket.append(static_cast<char>(0x40));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(98));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));

        quint32 sum = 10 + 21 + 64 + 0 + 0 + 98 + 0 + 0 + 0 + 0 + 0;
        mockPacket.append(static_cast<char>(sum & 0xFF));

        // Önce sahte veriyi enjekte et
        manager.injectTestData(mockPacket);

        // Beklenen sonuç: Sensör koptuğu için Saturation değerinin 0'a çekilmesi[cite: 4, 5]
        QCOMPARE(manager.saturation(), 0);
    }

    //Senaryo 3: random üretilen geçerli bir SpO2 değerini test etme
    void test_rastgele_veri_okuma() {
        SmmManager manager;

        //50 ile 100 arasında rastgele bir SpO2 değeri üret
        quint8 rastgeleSpo2 = static_cast<quint8>(QRandomGenerator::global()->bounded(50,101));
        QByteArray mockPacket;

        mockPacket.append(static_cast<char>(0xAA));
        mockPacket.append(static_cast<char>(0x55));
        mockPacket.append(static_cast<char>(10));
        mockPacket.append(static_cast<char>(21));



        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(rastgeleSpo2));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));

        //CheckSum hesaplaması(Len + Code + Data byte'larının toplamı)
        quint32 sum = 10 + 21 + 0 + 0 + rastgeleSpo2 + 0 + 0 + 0 + 0 + 0;
        mockPacket.append(static_cast<char>(sum & 0xFF));

        // Paketi modüle gönder[cite: 4]
        manager.injectTestData(mockPacket);

        // Beklenen sonuç: Saturation değeri, ürettiğimiz rastgele değere eşit olmalı[cite: 4, 5]
        QCOMPARE(manager.saturation(), rastgeleSpo2);
    }

    //Senaryo 4: Yanlış haber ile gelen paket görmezden gelinmeli
    void test_gecersiz_header() {
        SmmManager manager;
        QByteArray mockPacket;

        mockPacket.append(static_cast<char>(0xFF)); //hatalı header 1
        mockPacket.append(static_cast<char>(0xFF)); //hatalı header 2
        mockPacket.append(static_cast<char>(10));
        mockPacket.append(static_cast<char>(21));

        for (int i = 0; i < 9; ++i) {
            mockPacket.append(static_cast<char>(0x00));
        }
        mockPacket.append(static_cast<char>(0x00));
        manager.injectTestData(mockPacket);

        //beklenen: paket ayrıştırılmamalı, saturation değişmemeli (0 veya başlangıç değeri)
        QCOMPARE(manager.saturation(), 0);
    }
    void test_hatali_checksum() {
        SmmManager manager;
        QByteArray mockPacket;

        mockPacket.append(static_cast<char>(0xAA));
        mockPacket.append(static_cast<char>(0x55));
        mockPacket.append(static_cast<char>(10));
        mockPacket.append(static_cast<char>(21));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(98));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0x00));
        mockPacket.append(static_cast<char>(0xFF)); //kasıtlı olarak yanlış checksum

        manager.injectTestData(mockPacket);

        //beklenen: bozuk checksum'lı paket işlenmemeli, saturation 98 olmamalı
        QVERIFY(manager.saturation() != 98);
    }
    // Senaryo 6: Paket iki parça halinde gelirse (seri porttan gerçekçi durum)
    void test_parcali_veri_birlesimi() {
        SmmManager manager;
        QByteArray fullPacket;

        fullPacket.append(static_cast<char>(0xAA));
        fullPacket.append(static_cast<char>(0x55));
        fullPacket.append(static_cast<char>(10));
        fullPacket.append(static_cast<char>(21));
        fullPacket.append(static_cast<char>(0x00));
        fullPacket.append(static_cast<char>(0x00));
        fullPacket.append(static_cast<char>(0x00));
        fullPacket.append(static_cast<char>(97));
        fullPacket.append(static_cast<char>(0x00));
        fullPacket.append(static_cast<char>(0x00));
        fullPacket.append(static_cast<char>(0x00));
        fullPacket.append(static_cast<char>(0x00));
        fullPacket.append(static_cast<char>(0x00));
        quint32 sum = 10 + 21 + 0 + 0 + 97 + 0 + 0 + 0 + 0 + 0;
        fullPacket.append(static_cast<char>(sum & 0xFF));


        // Paketi ortadan ikiye böl ve ayrı ayrı gönder
        QByteArray firstHalf = fullPacket.left(7);
        QByteArray secondHalf = fullPacket.mid(7);

        manager.injectTestData(firstHalf);
        // Bu noktada paket henüz tamamlanmadı, saturation değişmemiş olmalı
        QCOMPARE(manager.saturation(), 0);

        manager.injectTestData(secondHalf);
        // Şimdi paket tamamlandı, doğru değer okunmalı
        QCOMPARE(manager.saturation(), 97);
    }
    // Senaryo 7: Sınır değerler (0 ve 100)
    void test_sinir_degerleri() {
        for (quint8 deger : {static_cast<quint8>(0), static_cast<quint8>(100)}) {
            SmmManager manager;
            QByteArray mockPacket;

            mockPacket.append(static_cast<char>(0xAA));
            mockPacket.append(static_cast<char>(0x55));
            mockPacket.append(static_cast<char>(10));
            mockPacket.append(static_cast<char>(21));
            mockPacket.append(static_cast<char>(0x00));
            mockPacket.append(static_cast<char>(0x00));
            mockPacket.append(static_cast<char>(0x00));
            mockPacket.append(static_cast<char>(deger));
            mockPacket.append(static_cast<char>(0x00));
            mockPacket.append(static_cast<char>(0x00));
            mockPacket.append(static_cast<char>(0x00));
            mockPacket.append(static_cast<char>(0x00));
            mockPacket.append(static_cast<char>(0x00));

            quint32 sum = 10 + 21 + 0 + 0 + deger + 0 + 0 + 0 + 0 + 0;
            mockPacket.append(static_cast<char>(sum & 0xFF));

            manager.injectTestData(mockPacket);
            QCOMPARE(manager.saturation(), deger);
        }
    }
 };

int main(int argc, char *argv[]) {
    //QML testleri görsel bileşenler içerdiği için QCoreApplication yerine QGuiApplication gerektirir
    QGuiApplication app(argc, argv);
    int status = 0;

    qDebug() << "C++ DONANIM MANTIK TESTLERI BASLIYOR";
    SmmTest cppTest;
    status |= QTest::qExec(&cppTest, argc, argv);

    qDebug() << "QML ARAYUZ TESTLERI BASLIYOR";
    //klasördeki "tst_" ile baslayan .qml dosyalarını otomatik baslatır
    status |= quick_test_main(argc, argv, "SmmQmlTestleri", QUICK_TEST_SOURCE_DIR);

    return status;
}


//test sınıfını çalıştıran makro(sınıf adıyla aynı olmalı)
#include "main.moc"
