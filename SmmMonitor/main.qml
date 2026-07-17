import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("SMM Module SpO2 Monitor")
    color: "#0f172a"

    //değerlere göre renk döndüren yardımcı fonksiyon
    function getStatusColor(val){
        if(val === 0) return "64748b"; //Gri(okuma yok)
        if(val >= 95) return "10b981"; // yeşil(sağlıklı)
        if(val >= 90) return "f59e0b"; // turuncu(uyarı)
        return "#ef4444"; //kırmızı(kritik/tehlike)
    }

    //değerlere göre bilgilendirme metni döndüren fonksiyon
    function getStatusText(val) {
        if (val === 0) return "Place Your Finger on the Sensor";
        if (val >= 95) return "Oxygen Level Normal";
        if (val >= 90) return "Low Oxygen Level";
        return "Critical Oxygen Level";
    }

    //verilerin gösterileceği Rectangle
    //Anakart
    Rectangle {
        id: infoRectangle
        width: 350
        height: 350
        anchors.centerIn: parent
        color: "#1e293b" //kart arka planı
        radius: 20
        border.width: 3
        border.color: getStatusColor(smmManager.saturation)

        //ölçüm yapılıyorsa çalışan parlama/nabız animasyonu
        Rectangle {
            anchors.fill: parent
            radius: 20
            color: "transparent"
            border.width: smmManager.saturation > 0 ? 8 : 0
            border.color: getStatusColor(smmManager.saturation)
            opacity: 0.2

            SequentialAnimation on opacity {
                loops: Animation.Infinite
                running: smmManager.saturation > 0 // sadece ölçüm varken çalışır

                NumberAnimation {to: 0.6; duration: 1000; easing.type:Easing.InOutQuad  }
                NumberAnimation {to: 0.1; duration: 1000; easing.type: Easing.InOutQuad }
            }
            Column{
                anchors.centerIn: parent
                spacing: 20

                Text {
                    text: "SATURATION - SpO2"
                    color: "#94a3b8"
                    font.pixelSize: 22
                    font.bold: true
                    font.letterSpacing: 1.5
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: smmManager.saturation === 0 ? "--" : "%" + smmManager.saturation
                    color: getStatusColor(smmManager.saturation)
                    font.pixelSize: 96
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: getStatusText(smmManager.saturation)
                    color: getStatusColor(smmManager.saturation)
                    font.pixelSize: 14
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }
}