import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: root
    width: 800
    height: 680
    visible: true
    title: qsTr("SMM Module SpO2 & Pulse Rate Monitor")
    color: "#0f172a"

    //değerlere göre renk döndüren yardımcı fonksiyon
    function getSpo2Color(val){
        if(val === 0) return "#b0e2ff"; //Gri(okuma yok)
        if(val >= 95) return "#10b981"; // yeşil(sağlıklı)
        if(val >= 90) return "#f59e0b"; // turuncu(uyarı)
        return "#ef4444"; //kırmızı(kritik/tehlike)
    }

    function getPulseColor (val){
        if (val === 0) return "#b0e2ff";
        if(val >= 60 && val <= 100) return "#0ea5e9";
        if (val > 100 || (val > 0 && val < 60)) return "#f59e0b";
        return "#ef4444";
    }

    //değerlere göre bilgilendirme metni döndüren fonksiyon
    function getStatusText(type, val) {
        if (val === undefined || val === null || val === 0) return "Place Your Finger on the Sensor";

        if(type === "spo2") {
            if (val >= 95) return "Oxygen Level Normal";
            if (val >= 90) return "Low Oxygen Level";
            return "Critical Oxygen Level";
        }
        if(type === "pulse") {
            if(val >= 60 && val <= 100) return "Pulse Rate Normal";
            if( val > 100 || (val > 0 && val < 60)) return "Abnormal Pulse Rate";
            return "Critical Pulse Rate";
        }
        return "--";
    }
    Text {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 15
        text: "Signal Normal"
        color: "#10b981"
        font.pixelSize: 14
        font.bold: true
        visible: !smmManager.isSignalWeak
    }

    Column{
        anchors.centerIn: parent
        spacing: 40

        Rectangle{
            width: 670
            height: 45
            color: "#ef4444"
            radius: 12
            anchors.horizontalCenter: parent.horizontalCenter
            visible: smmManager.isSignalWeak
            Row{
                anchors.centerIn: parent
                spacing: 10
                Text{
                    text:  "CRITICAL: WEAK SIGNAL DETECTED!"
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.bold: true
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 30


            //1.kutu: spo2
            Rectangle {
                width: 320
                height: 320
                color: "#473c8b"
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
                        color: "#b0e2ff"
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
                        text: getStatusText("spo2", smmManager.saturation)
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
                color: "#473c8b"
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
                        color: "#b0e2ff"
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
                        text: getStatusText("pulse", smmManager.pulseRate)
                        color: getPulseColor(smmManager.pulseRate)
                        font.pixelSize: 14
                        font.bold: true
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }
        ListView {
            id: historyList
            width: 670
            height: 200
            spacing: 8
            clip: true
            anchors.horizontalCenter: parent.horizontalCenter

            model: smmManager.getHistoryModel()


            delegate: Rectangle{
                width: historyList.width
                height: 45
                color: "#8b658b"
                radius: 20
                border.width: 1
                border.color: "#334155"

                Row{
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 50

                    Text {
                        text: model.timestamp !== undefined ? model.timestamp : " " //veritabanından gelen zamanlayıcı
                        color: "#473c8b"
                        font.pixelSize: 14
                        font.bold: true
                    }
                    Text{
                        text: "SpO2: %" + model.spo2 //veritabanından gelen spo2
                        color: root.getSpo2Color(model.spo2 !== undefined ? model.spo2 : "--")
                        font.pixelSize: 14
                        font.bold: true
                    }
                    Text {
                        text:"Pulse: " + model.pulseRate + " bpm" //veritabanından gelen pulse rate
                        color: root.getPulseColor(model.pulseRate !== undefined ? model.pulseRate : "--")
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
            }
        }
    }
}