import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle{
    id: root
    width: 100
    height: 49
    color: "#473c8b"
    radius: 10
    border.color: "#b0e2ff"
    border.width: 1

    signal openDatabase()

    Text{
        text: "Options"
        color: "#ffffff"
        font.pixelSize: 14
        font.bold: true
        anchors.centerIn: parent
    }
    MouseArea{
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            optionsPopup.open()
        }
    }
    Popup {
        id: optionsPopup
        width: 250
        height: 200

        x: root.width - width
        y:root.height + 10

        modal: true
        focus: true

        background: Rectangle{
            color: "#1e293b"
            radius: 20
            border.color: "#b0e2ff"
            border.width: 2
        }
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 25
            spacing: 15

            //başlık
            Text{
                text: "OPTIONS"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 15
                Layout.alignment: Qt.AlignHCenter
            }
            //settings
            Text {
                text: "Settings"
                color: "#0ea5e9"
                font.bold: true
                font.pixelSize: 15
                Layout.alignment: Qt.AlignHCenter

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        freqRow.visible = !freqRow.visible
                    }
                }
            }
            //açılır kapanır frekans drpdown kutusu
            RowLayout {
                id: freqRow
                visible: false //baslangicta gizli
                Layout.alignment: Qt.AlignLeft
                Layout.leftMargin: 15

                Text{
                    text: "Freq: "
                    color: "#473c8b"
                    font.bold: true
                }
                ComboBox {
                    id: freqComboBox
                    model: ["50 Hz", "60 Hz"]
                    Layout.preferredWidth:  85
                    currentIndex: smmManager.frequency === 60 ? 1 : 0

                    onActivated: function(index) {
                        if( index === 0) {
                            smmManager.setFrequency(50);
                        } else {
                            smmManager.setFrequency(60);
                        }
                    }
                }
            }
            //ayırıcı çizgi
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#473c8b"
                Layout.topMargin: 5
                Layout.bottomMargin: 5
            }
            // show the data butonu
            Text{
                text: "Show the Data"
                color: "#0ea5e9"
                font.bold: true
                font.pixelSize: 15
                Layout.alignment: Qt.AlignLeft

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        root.openDatabase()
                        optionsPopup.close()
                    }
                }
            }
            Item{
                Layout.fillHeight: true
            }
        }
    }
}
