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
        width: 260
        height: 310
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
                //TEST MENUSU

                Rectangle{
                    id:testMenuBtn
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 5
                    width: 120
                    height: 30
                    color: "transparent"
                    border.color: "#ef4444"
                    border.width: 1
                    radius: 5

                    Text{
                        text:"Test Menüsü ▼"
                        color: "#ef4444"
                        anchors.centerIn: parent
                        font.pixelSize: 11
                        font.bold: true
                    }
                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked:  {
                            testDropdown.open()
                        }
                    }
                    Menu {
                        id:testDropdown
                        y: 35

                        MenuItem {
                            text: "▶ Otomatik Simülasyon (2 sn)"
                            onClicked: {
                                smmManager.setSimulationMode(true)
                                smmSimulator.startSimulation()
                            }
                        }
                        MenuItem {
                            text: "⏹ Otomatik Simülasyonu Durdur"
                            onClicked: {
                                smmSimulator.stopSimulation()
                                smmManager.setSimulationMode(false)
                            }
                        }
                        MenuSeparator {} //araya imce çizgi çeker
                        MenuItem {
                            text: "Manuel: Normal Değerler"
                            onClicked: smmSimulator.forceState(0)
                        }
                        MenuItem {
                            text: "Manuel: Kritik Değerler"
                            onClicked: smmSimulator.forceState(1)
                        }
                        MenuItem {
                            text: "Manuel: Zayıf Sinyal"
                            onClicked: smmSimulator.forceState(2)
                        }
                        MenuItem {
                            text: "Manuel: Sensör Koptu"
                            onClicked: smmSimulator.forceState(3)
                        }
                        MenuItem{
                            text: "Manuel: Nabız Aranıyor"
                            onClicked: smmSimulator.forceState(4)
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
                Item{
                    Layout.fillHeight: true //menüyü yukarı yaslar
                }
            }
        }
    }
}
