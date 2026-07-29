import QtQuick 2.15
import QtQuick.Window 2.15
import CustomControls 1.0
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Backend 1.0

Window {
    id: root
    width: 800
    height: 680
    visible: true
    title: qsTr("SMM Module SpO2 & Pulse Rate Monitor")
    color: "#0f172a"

    property bool isMeasuringSpo2: false
    property bool isMeasuringPulse: false


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
    function getStatusText(type, val, isPortConnected, pulseSearch, beepVoice) {
        if(type === "pulse" && isPortConnected && pulseSearch) {
            return "Searching for Pulse...";
        }
        if(val === undefined || val === null || val === 0){
            if(beepVoice){
                return "Measuring...";
            }

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
        id: topHeaderRow
        anchors.top: parent.top
        anchors.left:parent.left
        anchors.margins: 15
        spacing: 40

        Column{
            spacing: 8
            anchors.verticalCenter: parent.verticalCenter

            Row{
                id:sensorStatusRow
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
            Text{
                text: "Signal Normal"
                color: "#10b981"
                font.pixelSize: 14
                font.bold: true
                visible: !smmManager.isSignalWeak && smmManager.isPortConnected
            }
        }
        Rectangle {
            width: 150
            height: 60
            color: "#1e293b"
            border.color: "#0ea5e9"
            border.width: 1
            radius: 8
            anchors.verticalCenter: parent.verticalCenter

            Column{
                anchors.centerIn: parent
                spacing: 5
                Text{
                    text: "Frequency: " + smmManager.frequency + " Hz"
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 13
                }
                Text{
                    text: "Mode: " + (smmManager.patientMode === SmmManager.Adult ? "Adult" :
                                                                                    (smmManager.patientMode === SmmManager.Newborn ? "Newborn" : "Pediatric"))
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 13

                }
            }
        }
    }

    OptionsButton {
        id: optionsMenu
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 15

        //optionsButton.qml içinden gönderilen siyali yakala ve veritabanını aç
        onOpenDatabase: {
            dbWindow.show()
        }
    }

    ColumnLayout {
        anchors.top: topHeaderRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 55
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
            Layout.bottomMargin: 30
            spacing: 30

            RowLayout{
                Layout.fillWidth: true
                Layout.preferredHeight: 280
                Layout.maximumHeight: 280
                Layout.minimumHeight: 250
                Layout.fillHeight: false
                spacing: 30

                //spo2 kartı
                Rectangle{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#473c8b"
                    radius: 20
                    border.width: 3
                    border.color: getSpo2Color(smmManager.saturation)
                    clip: true

                    Text{
                        text: "🫁"
                        font.pixelSize: 40
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.margins: 20
                        visible: smmManager.beepVoice
                    }
                    Column {
                        anchors.centerIn: parent
                        spacing: 10
                        Text{
                            text: "SpO2 (%)"
                            color: "#b0e2ff"
                            font.pixelSize: 24
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Text{
                            text: smmManager.saturation === 0 ? "--" : smmManager.saturation
                            color: getSpo2Color(smmManager.saturation)
                            font.pixelSize: 84
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                    Text{
                        text: getStatusText("spo2", smmManager.saturation, smmManager.isPortConnected, smmManager.pulseSearch, isMeasuringSpo2)
                        color: getSpo2Color(smmManager.saturation)
                        font.bold: true
                        font.pixelSize: 13
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottomMargin: 15
                    }
                }
                //Pulse Kartı
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#473c8b"
                    radius: 20
                    border.width: 3
                    border.color: getPulseColor(smmManager.pulseRate)
                    clip: true

                    Text{
                        text: "🩷"
                        font.pixelSize: 40
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.margins: 20
                        visible: smmManager.beepVoice
                    }
                    Column {
                        anchors.centerIn: parent
                        spacing: 10
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
                            font.pixelSize: 84
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                    Text{
                        text: getStatusText("pulse", smmManager.pulseRate, smmManager.isPortConnected, smmManager.pulseSearch, isMeasuringPulse)
                        color: getPulseColor(smmManager.pulseRate)
                        font.pixelSize: 13
                        font.bold: true
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottomMargin: 15
                    }
                }
            }

            Rectangle{
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#473c8b"
                radius: 20
                border.width: 3

                border.color: smmManager.isPortConnected ? "#19b981" : "#334155"
                clip: true

                RowLayout{
                    anchors.fill: parent
                    spacing: 0

                    Rectangle {
                        Layout.fillHeight: true
                        Layout.topMargin: 20
                        Layout.bottomMargin: 20
                        Layout.leftMargin: 15
                        width: 2
                        color: "#334155"
                        radius: 1
                    }
                    WaveformPlotter {
                        id: wavePlotter
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.topMargin: 15
                        Layout.bottomMargin: 15
                        Layout.rightMargin: 15
                        Layout.leftMargin: 5
                    }
                }
                Text{
                    text: "Plethysmogram (PPG)"
                    color: "#64748b"
                    font.pixelSize: 12
                    font.bold: true
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.margins: 15
                }
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
        function onBeepVoiceChanged(newBeepVoice){
            if(newBeepVoice){
                if(smmManager.saturation === 0) isMeasuringSpo2 = true;
                if(smmManager.pulseRate === 0) isMeasuringPulse = true;

                measuringTimeoutTimer.restart();
            }
        }
        function onSaturationChanged(newSaturation) {
            if(newSaturation > 0) isMeasuringSpo2 = false;
        }
        function onPulseRateChanged(newPulseRate) {
            if(newPulseRate > 0) isMeasuringPulse = false;
        }
        //sensör bağlantısı kesilirse ölçüm durumlarını sıfırla
        function onIsPortConnectedChanged(connected) {
            if(!connected){
                isMeasuringSpo2 = false;
                isMeasuringPulse = false;
            }
        }
    }
    Timer{
        id:measuringTimeoutTimer
        interval: 1500
        repeat: false
        onTriggered: {
            isMeasuringSpo2 = false;
            isMeasuringPulse = false;
            console.log("Sensor idle: Measurement status reset by timeout.");
        }
    }
}



