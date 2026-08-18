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
    property bool isDemoActive: false

    property bool demoBlink: false

    Component.onCompleted: {
        smmManager.setTargetIp(ipInput.text);

        console.log("averageSecond:", smmManager.averageSecond,
                    "sec4:", SmmManager.sec4,
                    "sec8:", SmmManager.sec8,
                    "sec16:", SmmManager.sec16)
        console.log("Adult:", SmmManager.Adult, "Newborn:", SmmManager.Newborn, "Pediatric:", SmmManager.Pediatric)
    }

    function stopDemoMode() {
        smmSimulator.stopDemo();
        smmManager.disconnectPort();
        isDemoActive = false;

        wavePlotter.clear();
        smmManager.setDemoMode(false);
    }


    //değerlere göre renk döndüren yardımcı fonksiyon
    function getSpo2Color(val){
        if(val === 0) return "#b0e2ff"; //Gri(okuma yok)
        if (val <= 80) return "#ef4444";
        // Değer 80'in üstünde ama kullanıcının belirlediği limitlerin dışındaysa, seçilen öncelik rengini yansıt
        if(val < smmManager.spo2LowerLimit || val > smmManager.spo2UpperLimit) {
            if (smmManager.spo2AlarmPriority === SmmManager.Blue) return "#3b82f6";
            if (smmManager.spo2AlarmPriority === SmmManager.Yellow) return "#eab308";
            return "#ef4444";
        }
        if(val <= smmManager.spo2LowerLimit + 2) return "#f59e0b";
        return "#10b981";
    }

    function getPulseColor (val){
        if (val === 0) return "#b0e2ff";
        if(val <= 40 || val >= 150) return "#ef4444";
        if(val < smmManager.pulseLowerLimit || val > smmManager.pulseUpperLimit) {
            if(smmManager.pulseAlarmPriority === SmmManager.Blue) return "#3b82f6";
            if(smmManager.pulseAlarmPriority === SmmManager.Yellow) return "#eab308";
            return "#ef4444";
        }
        return "#0ea5e9";
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


    Timer {
        id: demoBlinkTimer
        interval: smmManager.pulseRate > 0 ? (6000 / smmManager.pulseRate) : 1000
        running: isDemoActive && smmManager.isPortConnected
        repeat: true
        onTriggered: {
            demoBlink = true;
            demoBlinkOffTimer.start();
            if (smmManager.saturation === 0) isMeasuringSpo2 = true;
            if (smmManager.pulseRate === 0) isMeasuringPulse = true;
            measuringTimeoutTimer.restart();
        }
    }

    Timer {
        id: demoBlinkOffTimer
        interval: 150
        repeat: false
        onTriggered: demoBlink = false
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
            Text{
                text: "Please select the correct port"
                color: "#94a3b8"
                font.pixelSize: 11
                font.bold: true
                visible: smmManager.hasConnectionError
            }

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
            width: 180
            height: 80
            color: "#1e293b"
            border.color: "#0ea5e9"
            border.width: 1
            radius: 8
            anchors.verticalCenter: parent.verticalCenter

            Column{
                anchors.centerIn: parent
                spacing: 4
                Text{
                    text: "Frequency: " + smmManager.frequency + " Hz"
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 12
                }
                Text{
                    text: "Mode: " + (smmManager.patientMode === SmmManager.Adult ? "Adult" :
                                                                                    (smmManager.patientMode === SmmManager.Newborn ? "Newborn" : "Pediatric"))
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 12

                }
                Text {
                    text: "Avg. Seconds: " + (smmManager.averageSecond === 4 ? "4 Sec" :
                                                                               (smmManager.averageSecond === 8 ? "8 Sec" : "16 Sec"))
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 12
                }
                Text {
                    text: "Waveform Speed: " + smmManager.waveformSpeed + "m/s"
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 12
                }
            }
        }
    }

    AlarmLogsWindow {
        id: alarmLogsWindow
    }

    Rectangle {
        id: muteAlarmButton
        anchors.top: parent.top
        anchors.right: optionsMenu.left
        anchors.margins: 15
        height: 40
        width:150
        radius: 8
        color: smmManager.isAlarmMuted ? "#f59e0b" : "#1e293b"
        border.color: smmManager.isAlarmMuted ? "#d97706" : "#0ea5e9"
        border.width: 1

        Text {
            anchors.centerIn: parent
            text: smmManager.isAlarmMuted ? "🔇 Alarm Muted (2m)" : "🔊 Mute Alarms (2m)"
            color: "#ffffff"
            font.bold: true
            font.pixelSize: 12
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                smmManager.muteAlarmForTwoMinutes();
            }
        }
    }

    OptionsButton {
        id: optionsMenu
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 15

        isDemoActive: root.isDemoActive

        //optionsButton.qml içinden gönderilen siyali yakala ve veritabanını aç
        onOpenDatabase: {
            dbWindow.show()
        }
        onOpenAlarmLogsWindow: {
            alarmLogsWindow.show();
        }
        onExportToPdf: {
            console.log("PDF dışarı aktarma tetiklendi.");
            smmManager.exportDataToPdf();
        }
        onExportToExcel: {
            console.log("Excel dışarı aktarma tetiklendi.");
            smmManager.exportDataToExcel();
        }
        onToggleDemoMode: {
            if(!isDemoActive) {
                smmSimulator.startDemo();
                root.isDemoActive = true;

                smmManager.setDemoMode(true);
            } else {
                stopDemoMode();
            }
        }
    }

    ColumnLayout {
        anchors.top: topHeaderRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 10
        spacing: 10

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
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 670
            height: 30
            color: {
                if (smmManager.saturation > 0 && smmManager.saturation <= 80) return "#ef4444";

                // Değilse kullanıcının seçtiği renk
                if (smmManager.spo2AlarmPriority === SmmManager.Blue) return "#3b82f6";
                if (smmManager.spo2AlarmPriority === SmmManager.Yellow) return "#eab308";
                return "#ef4444";
            }

            radius: 12
            visible: smmManager.isSpo2AlarmActive && smmManager.isPortConnected

            SequentialAnimation on opacity {
                loops: Animation.Infinite
                running: smmManager.isSpo2AlarmActive
                NumberAnimation {to: 0.3; duration: 600}
                NumberAnimation {to: 1.0; duration: 600}
            }
            Row {
                anchors.centerIn: parent
                spacing: 10
                Text {
                    text: "⚠️ ALARM: SpO2 Value is Out of Range!"
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.bold: true
                }
            }
        }
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 670
            height: 30
            color: {
                if (smmManager.pulseRate > 0 && smmManager.pulseRate <= 40 || smmManager.pulseRate >= 150) return "#ef4444";

                // Değilse kullanıcının seçtiği renk
                if (smmManager.pulseAlarmPriority === SmmManager.Blue) return "#3b82f6";
                if (smmManager.pulseAlarmPriority === SmmManager.Yellow) return "#eab308";
                return "#ef4444";
            }

            radius: 12
            visible: smmManager.isPulseAlarmActive && smmManager.isPortConnected

            SequentialAnimation on opacity {
                loops: Animation.Infinite
                running: smmManager.isPulseAlarmActive
                NumberAnimation {to: 0.3; duration: 600}
                NumberAnimation {to: 1.0; duration: 600}
            }
            Row {
                anchors.centerIn: parent
                spacing: 10
                Text {
                    text: "⚠️ ALARM: Pulse Value is Out of Range!"
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
                    Button {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: 15
                        text: "⚙"
                        font.pixelSize: 15
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                spo2SettingsPopup.open()
                            }
                        }
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
                    Button {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: 15
                        text: "⚙"
                        font.pixelSize: 15
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                pulseSettingsPopup.open()
                            }
                        }
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

                        onWidthChanged: {
                            wavePlotter.calibrate(Screen.pixelDensity)
                        }

                        Component.onCompleted: {
                            wavePlotter.calibrate(Screen.pixelDensity)
                        }
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

                if(!smmManager.isAlarmMuted && !smmManager.isSpo2AlarmActive && !smmManager.isPulseAlarmActive) {
                    // SmmManager içerisindeki Q_ENUM(SoundType) listesinden Info(2) sesini tetikler
                    smmManager.playSoundEffect(1);
                }

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
                wavePlotter.clear();
            }
        }
        function onWaveformSpeedChanged(newSpeed) {
            wavePlotter.setWaveformSpeed(newSpeed)
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
    Popup {
        id: spo2SettingsPopup
        x: Math.round((root.width - width) / 2)
        y: Math.round((root.height - height) / 2)
        width: 320
        height: 220
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            color: "#1e293b"
            border.color: "#0ea5e9"
            border.width: 2
            radius: 8
        }


        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 15

            Text {
                text: "SpO2 Alarm Settings"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 15
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: "Lower Limit (%):"
                    color: "#94a3b8"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true }
                SpinBox {
                    from: 50
                    to: 100
                    editable: true
                    value: smmManager.spo2LowerLimit
                    onValueChanged: {
                        if(value !== smmManager.spo2LowerLimit)
                            smmManager.spo2LowerLimit = value;
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Text { text: "Upper Limit (%):"
                    color: "#94a3b8"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                }
                SpinBox {
                    from: 50
                    to: 100
                    editable: true
                    value: smmManager.spo2UpperLimit
                    onValueChanged: {
                        if(value !== smmManager.spo2UpperLimit) smmManager.spo2UpperLimit = value;
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Text { text: "Priority Color:"
                    color: "#94a3b8"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                }
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    color: "#3b82f6"
                    border.color: "#ffffff"
                    border.width: smmManager.spo2AlarmPriority === SmmManager.Blue ? 3 : 0;
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: smmManager.spo2AlarmPriority = SmmManager.Blue
                    }
                }
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    color: "#eab308"
                    border.color: "#ffffff"
                    border.width: smmManager.spo2AlarmPriority === SmmManager.Yellow ? 3 : 0
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: smmManager.spo2AlarmPriority = SmmManager.Yellow
                    }
                }
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    color: "#ef4444"
                    border.color: "#ffffff"
                    border.width: smmManager.spo2AlarmPriority === SmmManager.Red ? 3 : 0
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: smmManager.spo2AlarmPriority = SmmManager.Red
                    }
                }
            }
        }
    }
    Popup {
        id: pulseSettingsPopup
        x: Math.round((root.width - width) / 2)
        y:  Math.round((root.height - height) / 2)
        width: 320
        height: 180
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle {
            color:"#1e293b"
            border.color: "#0ea5e9"
            border.width: 2
            radius: 8
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 15

            Text { text: "Pulse Alarm Settings"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 15
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                Layout.fillWidth: true
                Text { text: "Lower limit (bpm):"
                    color: "#94a3b8"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                }
                SpinBox {
                    from: 30
                    to: 250
                    editable: true
                    value: smmManager.pulseLowerLimit
                    onValueChanged: {
                        if(value !== smmManager.pulseLowerLimit)
                            smmManager.pulseLowerLimit = value;
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Text { text: "Upper limit (bpm):"
                    color: "#94a3b8"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                }
                SpinBox {
                    from: 30
                    to: 250
                    editable: true
                    value: smmManager.pulseUpperLimit
                    onValueChanged: {
                        if(value !== smmManager.pulseUpperLimit)
                            smmManager.pulseUpperLimit = value;
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Text { text: "Priority Color:"
                    color: "#94a3b8"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                }
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    color: "#3b82f6"
                    border.color: "#ffffff"
                    border.width: smmManager.pulseAlarmPriority === SmmManager.Blue ? 3 : 0;
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: smmManager.pulseAlarmPriority = SmmManager.Blue
                    }
                }
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    color: "#eab308"
                    border.color: "#ffffff"
                    border.width: smmManager.pulseAlarmPriority === SmmManager.Yellow ? 3 : 0
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: smmManager.pulseAlarmPriority = SmmManager.Yellow
                    }
                }
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    color: "#ef4444"
                    border.color: "#ffffff"
                    border.width: smmManager.pulseAlarmPriority === SmmManager.Red ? 3 : 0
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: smmManager.pulseAlarmPriority = SmmManager.Red
                    }
                }
            }
        }
    }
    //IP GİRİŞ ALANI
    Row {
        id: serverIpRow
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: 8
        anchors.leftMargin: 40
        spacing : 8
        z : 99 //diğer katmanların altında kalmaması için en üste alıyoruz
        Text {
            text: "Server IP:"
            color: "#64748b"
            font.pixelSize: 12
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            width: 120
            height: 20
            color: "#1e293b"
            border.color: "#334155"
            border.width: 1
            radius: 4
            anchors.verticalCenter: parent.verticalCenter
            clip: true

            TextInput {
                id: ipInput
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                verticalAlignment: TextInput.AlignVCenter
                color: "#ffffff"
                font.pixelSize: 12

                // Placeholder yerine başlangıç değeri veriyoruz
                text: "192.168.5.149"

                // Etrafındaki siyah seçim çizgisini kaldırır
                selectByMouse: true
                selectionColor: "#0ea5e9"

                onEditingFinished: {
                    // Odak kaybolduğunda veya Enter'a basıldığında tetiklenir
                    smmManager.setTargetIp(text)
                    console.log("Hedef IP ayarlandı: " + text)
                }
            }
        }
    }
}



