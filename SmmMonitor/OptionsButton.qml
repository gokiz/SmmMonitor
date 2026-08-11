import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Backend 1.0

Rectangle {
    id: root
    width: 100
    height: 50
    color: "#473c8b"
    radius: 10

    signal openDatabase()
    signal openAlarmLogsWindow()
    signal exportToPdf()
    signal exportToExcel()
    signal toggleDemoMode()

    property bool isDemoActive: false

    Text{
        text: "OPTIONS"
        color:"#ffffff"
        font.pixelSize: 14
        font.bold: true
        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            optionsPopup.open()
        }
    }
    Popup {
        id:optionsPopup
        width: 270
        height: 400
        x: root.width - width
        y: root.height + 10

        modal: true
        focus: true

        background: Rectangle{
            color: "#1e293b"
            radius: 20
            border.color: "#b0e2ff"
            border.width: 2
        }
        ScrollView {
            anchors.fill: parent
            anchors.margins: 15
            clip: true

            contentWidth: availableWidth
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ColumnLayout {
                width: parent.width
                spacing: 15

                Text{
                    text: "OPTIONS"
                    color:"#ffffff"
                    font.bold: true
                    font.pixelSize: 15
                    Layout.alignment: Qt.AlignHCenter
                }
                Text{
                    text: "Settings"
                    color: "#0ea5e9"
                    font.bold: true
                    font.pixelSize: 15
                    Layout.alignment: Qt.AlignHCenter

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            freqRow.visible = !freqRow.visible
                            patients.visible = !patients.visible
                            avgseconds.visible = !avgseconds.visible
                            waveformSppedRow.visible = !waveformSppedRow.visible
                        }
                    }
                }

                //açılır kapanır frekans dropdown
                RowLayout {
                    id:freqRow
                    visible: false
                    Layout.alignment: Qt.AlignLeft

                    Text{
                        text:"Frequency: "
                        color: "#473c8b"
                        font.bold: true
                    }
                    ComboBox {
                        id:freqComboBox
                        model: ["50 Hz", "60 Hz"]
                        Layout.preferredWidth: 85
                        currentIndex: smmManager.frequency === 60 ? 1 : 0

                        onActivated: function(index) {
                            if(index === 0){
                                smmManager.setFrequency(50);
                            } else {
                                smmManager.setFrequency(60);
                            }
                        }
                    }
                }
                RowLayout {
                    id: patients
                    visible: false
                    Layout.alignment: Qt.AlignLeft

                    Text {
                        text: "Patient Mode: "
                        color: "#473c8b"
                        font.bold: true

                    }
                    ComboBox {
                        id: patientsComboBox
                        model: ["Adult", "Newborn", "Pediatric"]
                        Layout.preferredWidth: 105
                        currentIndex: smmManager.patientMode === SmmManager.Adult ? 0 :
                                                                                    (smmManager.patientMode === SmmManager.Newborn ? 1 : 2)
                        onActivated: function(index) {
                            if (index === 0) {
                                smmManager.setPatientMode(SmmManager.Adult);
                            } else if (index === 1) {
                                smmManager.setPatientMode(SmmManager.Newborn);
                            } else if (index === 2) {
                                smmManager.setPatientMode(SmmManager.Pediatric);
                            }
                        }
                    }
                }
                RowLayout {
                    id:avgseconds
                    visible: false
                    Layout.alignment: Qt.AlignLeft

                    Text{
                        text: "Average Seconds: "
                        color: "#473c8b"
                        font.bold: true
                    }
                    ComboBox{
                        id: avgSecondsComboBox
                        model: ["4 Seconds", "8 Seconds", "16 seconds"]
                        Layout.preferredWidth: 100
                        currentIndex: smmManager.averageSecond === 4 ? 0 :
                                                                       (smmManager.averageSecond === 8 ? 1 : 2)

                        onActivated: function(index) {
                            if(index === 0){
                                smmManager.setAverageSecond(4);
                            }else if (index === 1) {
                                smmManager.setAverageSecond(8);
                            }else if(index === 2) {
                                smmManager.setAverageSecond(16);
                            }
                        }
                    }
                }
                //waveform hız ayarı
                RowLayout {
                    id: waveformSppedRow
                    visible: false
                    Layout.alignment: Qt.AlignLeft
                    spacing: 10

                    Text {
                        text: "Waveform Speed: "
                        color: "#473c8b"
                        font.bold: true
                    }
                    ComboBox {
                        id: waveformSpeedComboBox
                        model: ["25 m/s", "50 m/s"]
                        Layout.preferredWidth: 105
                        currentIndex: {
                            if (smmManager.waveformSpeed === 50) return 1;
                            return 0; // Varsayılan veya 25 için
                        }

                        onActivated:  function(index) {
                            if(index === 0) {
                                smmManager.setWaveformSpeed(25);
                            } else {
                                smmManager.setWaveformSpeed(50);
                            }
                        }
                    }
                }

                //ayırıcı çizgi
                Rectangle{
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#473c8b"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }
                //show data butonu
                Text{
                    text: "Show the Data"
                    color: "#0ea5e9"
                    font.bold: true
                    font.pixelSize: 15
                    Layout.alignment: Qt.AlignHCenter

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.openDatabase()
                            optionsPopup.close()
                        }
                    }
                }
                Rectangle{
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#473c8b"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                ColumnLayout{
                    Layout.alignment: Qt.AlignHCenter
                    spacing:5

                    Text {
                        text: "Port"
                        color: "#0ea5e9"
                        font.bold: true
                        font.pixelSize: 15
                        Layout.alignment: Qt.AlignHCenter

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                portComboBox.visible = !portComboBox.visible;
                                if(portComboBox.visible){
                                    smmManager.refreshPorts();
                                }
                            }
                        }
                    }
                    ComboBox{
                        id:portComboBox
                        visible: false
                        model:smmManager.availablePorts
                        Layout.preferredWidth: 110
                        Layout.alignment: Qt.AlignHCenter

                        onActivated: function(index) {
                            var selectedPort = portComboBox.textAt(index);
                            smmManager.connectToModule(selectedPort);
                        }
                    }
                }

                Rectangle{
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#473c8b"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }


                Text {
                    text: "Alarm Log History"
                    color: "#0ea5e9"
                    font.bold: true
                    font.pixelSize: 15
                    Layout.alignment: Qt.AlignHCenter

                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.openAlarmLogsWindow()
                            optionsPopup.close()
                        }
                    }
                }
                Rectangle{
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#473c8b"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }
                Text {
                    text: "📄 PDF: Alarm Records"
                    color: "#0ea5e9"
                    font.bold: true
                    font.pixelSize: 15
                    Layout.alignment: Qt.AlignHCenter

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.exportToPdf()
                            optionsPopup.close()
                        }
                    }
                }
                Rectangle{
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#473c8b"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                Text {
                    text: "📊 Excel: All Measurement Records"
                    color: "#0ea5e9"
                    font.bold: true
                    font.pixelSize: 15
                    Layout.alignment: Qt.AlignHCenter

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.exportToExcel()
                            optionsPopup.close()
                        }
                    }
                }
                Rectangle{
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#473c8b"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                Text {
                    id: demoTextItem
                    // Sabit bir alan verelim ki diğer yazılarla üst üste binmesin
                    Layout.preferredHeight: 35
                    Layout.alignment: Qt.AlignHCenter

                    text: root.isDemoActive ? "Stop Demo" : "Start Demo"
                    color: root.isDemoActive ? "#ef4444" : "#0ea5e9"
                    font.bold: true
                    font.pixelSize: 15

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.toggleDemoMode()
                            optionsPopup.close()
                        }
                    }
                }

                Item{
                    Layout.fillHeight: true //menüyü yukarı yaslar
                }
            }
        }
    }
}
