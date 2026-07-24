import QtQuick 2.15
import QtQuick.Window 2.15
import CustomControls 1.0
import QtQuick.Layouts 1.15

Window {
    id: root
    width: 800
    height: 680
    visible: true
    title: qsTr("SMM Module SpO2 & Pulse Rate Monitor")
    color: "#0f172a"


    Row{
        visible: false
        anchors.top: parent.top
        anchors.right: dbButton.left //show data butonunun solu
        anchors.margins: 15
        spacing: 10
        //testi baslat
        Rectangle {
            width: 70
            height: 30
            color: "transparent"
            border.color: "#ef4444"
            border.width: 1
            radius: 5
            Text {
                text: "Test Başlat"
                color: "#ef4444"
                anchors.centerIn: parent
                font.pixelSize: 11
                font.bold: true
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    smmManager.setSimulationMode(true);
                    //simulasyon başlatma komutu
                    smmSimulator.startSimulation();
                    // Port bağlıymış gibi gösterelim ki "Sensörü Açın" uyarıları vs düzgün çalışsın
                    // (smmmanager.cpp'de isPortConnected'ı test için public yaptıysan veya bir test fonksiyonu varsa)
                }
            }
        }
        //testi bitir
        Rectangle {
            width: 70
            height: 30
            color: "transparent"
            border.color: "#6b7280"
            border.width: 1
            radius: 5
            Text {
                text: "Test Bitir"
                color: "#6b7280"
                anchors.centerIn: parent
                font.pixelSize: 11
                font.bold: true
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    smmSimulator.stopSimulation();
                    smmManager.setSimulationMode(false);
                }
            }
        }
    }

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
    function getStatusText(type, val, isPortConnected, pulseSearch) {
        if(type === "pulse" && isPortConnected && pulseSearch) {
            return "Searching for Pulse...";
        }
        if(val === undefined || val === null || val === 0){
            return "Place Your Finger on the Sensor";
        }
        if(type === "spo2"){
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
    //sensör açık/kapalı gösterimi
    Row {
        id:sensorStatusRow
        anchors.top: parent.top
        anchors.left:parent.left
        anchors.margins: 15
        spacing: 10
        Rectangle {
            width:12
            height: 12
            radius: 6
            color: smmManager.isPortConnected ? "#10b981" : "#ef4444"
            anchors.verticalCenter: parent.verticalCenter

            SequentialAnimation on opacity {
                loops: Animation.Infinite
                running: !smmManager.isPortConnected
                NumberAnimation {to: 0.2; duration: 500}
                NumberAnimation {to: 1.0; duration: 500}
            }
        }
        Text{
            text: smmManager.isPortConnected ? "Sensor On" : "Sensor Off"
            color: smmManager.isPortConnected ? "#10b981" : "#ef4444"
            font.pixelSize: 14
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    //Signal yazısı
    Text{
        anchors.top: sensorStatusRow.bottom
        anchors.left:parent.left
        anchors.topMargin: 8 //iki yazı arası boşluk
        anchors.leftMargin: 15
        text:"Signal Normal"
        color: "#10b981"
        font.pixelSize: 14
        font.bold: true
        //hem sinyal zayıf olmayacak hem de port bağlı olacak
        visible: !smmManager.isSignalWeak && smmManager.isPortConnected
    }
    //veritabanı butonu
    Rectangle{
        id: dbButton
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 15
        width:130
        height:49
        color: "#473c8b"
        radius: 10
        border.color:"#b0e2ff"
        border.width: 1

        Text{
            text:"Show the Data"
            color: "#ffffff"
            font.pixelSize: 14
            font.bold: true
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor // fare ile üzerine gelince el gbi görünsün diye
            onClicked: {
                dbWindow.show()
            }
        }
    }
    ColumnLayout {
        anchors.top: sensorStatusRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 30
        spacing: 30

        Rectangle{
            Layout.alignment: Qt.AlignHCenter
            width: 670
            height: 45
            color: "#ef4444"
            radius: 12
            visible: smmManager.isSignalWeak
            Row {
                anchors.centerIn: parent
                spacing: 10
                Text{
                    text: "CRITICAL: WEAK SIGNAL DETECTED!"
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.bold: true
                }
            }
        }
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "Turn on the Sensor"
            color: "#ef4444"
            font.pixelSize: 24
            font.bold: true
            visible: !smmManager.isPortConnected
        }
        ColumnLayout{
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 40
            Layout.rightMargin: 40
            spacing: 30

            Rectangle{
                Layout.fillWidth: true
                Layout.preferredHeight: 250
                color: "#473c8b"
                radius: 20
                border.width: 3
                border.color: getSpo2Color(smmManager.saturation)
                clip: true

                RowLayout{
                    anchors.fill: parent
                    spacing: 0

                    Item {
                        Layout.preferredWidth: 240
                        Layout.fillHeight: true

                        Text{
                            text: "🫁"
                            font.pixelSize: 40
                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.margins: 20
                            visible: smmManager.beepVoice
                        }
                        Rectangle {
                            anchors.fill: parent
                            radius: 20
                            color:"transparent"
                            border.width: smmManager.saturation > 0 ? 8 : 0
                            border.color: getSpo2Color(smmManager.saturation)
                            opacity: 0.2

                            SequentialAnimation on opacity{
                                loops: Animation.Infinite
                                running: smmManager.saturation > 0
                                NumberAnimation{ to: 0.6; duration: 1000; easing.type: Easing.InOutQuad }
                                NumberAnimation{ to: 0.1; duration: 1000; easing.type: Easing.InOutQuad }
                            }
                        }
                        Column{
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
                                text: getStatusText("spo2", smmManager.saturation, smmManager.isPortConnected, smmManager.pulseSearch)
                                color: getSpo2Color(smmManager.saturation)
                                font.pixelSize: 14
                                font.bold: true
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }

                    Rectangle{
                        Layout.fillHeight: true
                        Layout.topMargin: 20
                        Layout.bottomMargin: 20
                        width: 2
                        color: "#334155"
                        radius: 1
                    }
                    // Y Ekseni Sayıları
                    ColumnLayout {
                        Layout.fillHeight: true
                        Layout.topMargin: 20
                        Layout.bottomMargin: 20
                        Layout.leftMargin: 10 // Çizgiden sonra hafif boşluk
                        spacing: 0

                        Text { text: "100"; color: "#64748b"; font.pixelSize: 11; font.bold: true; Layout.alignment: Qt.AlignTop }
                        Item { Layout.fillHeight: true } // Sayıların arasını eşit açmak için görünmez yay
                        Text { text: " 50"; color: "#64748b"; font.pixelSize: 11; font.bold: true; Layout.alignment: Qt.AlignVCenter }
                        Item { Layout.fillHeight: true }
                        Text { text: "  0"; color: "#64748b"; font.pixelSize: 11; font.bold: true; Layout.alignment: Qt.AlignBottom }
                    }

                    WaveformPlotter {
                        id: wavePlotter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.topMargin: 15
                        Layout.bottomMargin: 15
                        Layout.rightMargin: 15
                        Layout.leftMargin: 5
                    }
                }
            }
            Rectangle {
                Layout.preferredWidth: 320 // Üstteki SpO2 metin kutusu ile aynı hizada kalır
                Layout.preferredHeight: 250
                color: "#473c8b"
                radius: 20
                border.width: 3
                border.color: getPulseColor(smmManager.pulseRate)

                Text {
                    text: "🩷"
                    font.pixelSize: 40
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.margins: 20
                    visible: smmManager.beepVoice
                }

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

                    Text {
                        text: "Pulse Rate (bpm)"
                        color: "#b0e2ff"
                        font.pixelSize: 24
                        font.bold: true
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: smmManager.pulseRate === 0 ? "--" : smmManager.pulseRate
                        color: getPulseColor(smmManager.pulseRate)
                        font.pixelSize: 96
                        font.bold: true
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: getStatusText("pulse", smmManager.pulseRate, smmManager.isPortConnected, smmManager.pulseSearch)
                        color: getPulseColor(smmManager.pulseRate)
                        font.pixelSize: 14
                        font.bold: true
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }
    DatabaseWindow {
        id: dbWindow
    }
    Connections {
        target: smmManager

        function onWaveformChanged(newWaveform) {
            wavePlotter.addPoint(newWaveform)
        }
    }
}



