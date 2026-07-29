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
        width: 250
        height: 250
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
                Item{
                    Layout.fillHeight: true //menüyü yukarı yaslar
                }
            }
        }
    }
}
