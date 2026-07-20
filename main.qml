import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 800
    height: 480
    visible: true
    title: qsTr("SMM Module SpO2 & Pulse Rate Monitor")
    color: "#0f172a"

    //değerlere göre renk döndüren yardımcı fonksiyon
    function getSpo2Color(val){
        if(val === 0) return "64748b"; //Gri(okuma yok)
        if(val >= 95) return "10b981"; // yeşil(sağlıklı)
        if(val >= 90) return "f59e0b"; // turuncu(uyarı)
        return "#ef4444"; //kırmızı(kritik/tehlike)
    }

    function getPulseColor (val){
        if (val === 0) return "#64748b";
        if(val >= 60 && val <= 100) return "#0ea5e9";
        if (val > 100 || (val > 0 && val < 60)) return "#f59e0b";
        return "#ef4444";
    }

    //değerlere göre bilgilendirme metni döndüren fonksiyon
    function getStatusText(val) {
        if (val === 0) return "Place Your Finger on the Sensor";

        if(type === "spo2") {
            if (val >= 95) return "Oxygen Level Normal";
            if (val >= 90) return "Low Oxygen Level";
            return "Critical Oxygen Level";
        }
        if(type === "pulse") {
            if(val >= 60 && val <= 100) return "Normal Pulse Rate";
            if( val > 100 || (val > 0 && val < 60)) return "Abnormal Pulse Rate";
            return "Critical Pulse Rate";
        }
    }

    Row {
        anchors.centerIn: parent
        spacing: 30


        //1.kutu: spo2
        Rectangle {
            width: 320
            height: 320
            color: "#1e293b"
            radius: 20
            border.width: 3
            border.color: getSpo2Color(smmManager.saturation)

            //nabız animasyon katmanı
            Rectangle {
                anchors.fill: parent
                radius: 20
                color: "transparent"
                border.width: smmManager.saturation > 0 ? 8 : 0
                border.color: getSpo2Color(smmManager.saturation)
                opacity: 0.2

                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    running: smmManager.saturation > 0
                    NumberAnimation { to: 0.6; duration: 1000; easing.type: Easing.InOutQuad }
                    NumberAnimation { to: 0.1; duration: 1000; easing.type: Easing.InOutQuad }
                }
            }
            Column {
                anchors.centerIn: parent
                spacing: 20

                Text {
                    text:"SpO2 (%)"
                    color: "#94a3b8"
                    font.pixelSize: 24
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: smmManager.saturation === 0 ? "--" : smmManager.saturation
                    color: getSpo2Color(smmManager.saturation)
                    font.pixelSize: 96
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: getStatusText(smmManager.saturation, "spo2")
                    color: getSpo2Color(smmManager.saturation)
                    font.pixelSize: 14
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
        //2.kutu-pulse
        Rectangle {
            width: 320
            height: 320
            color: "#1e293b"
            radius: 20
            border.width: 3
            border.color: getPulseColor(smmManager.pulseRate)

            //nabız animasyonu
            Rectangle {
                anchors.fill: parent
                radius: 20
                color: "transparent"
                border.width: smmManager.pulseRate > 0 ? 8 : 0
                border.color: getPulseColor(smmManager.pulseRate)
                opacity: 0.2

                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    running: smmManager.pulseRate > 0
                    NumberAnimation { to: 0.6; duration: 1000; easing.type: Easing.InOutQuad}
                    NumberAnimation { to: 0.1; duration: 1000; easing.type: Easing.InOutQuad}
                }
            }
            Column {
                anchors.centerIn: parent
                spacing: 20
                Text{
                    text: "Pulse Rate (bpm)"
                    color: "#94a3b8"
                    font.pixelSize: 24
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text{
                    text: smmManager.pulseRate === 0 ? "--" : smmManager.pulseRate
                    color: getPulseColor(smmManager.pulseRate)
                    font.pixelSize: 96
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter

                }
                Text{
                    text: getStatusText(smmManager.pulseRate, "pulse")
                    color: getPulseColor(smmManager.pulseRate)
                    font.pixelSize: 14
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }
}