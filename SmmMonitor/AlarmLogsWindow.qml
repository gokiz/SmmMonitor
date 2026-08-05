import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: alarmLogsWindow
    width: 800
    height: 680
    title: "Patient Alarm Logs History"
    color: "#0f172a" // Doğru arkaplan rengi

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: "Recorded Alarm Events (Based on Custom Limits)"
                color: "#ffffff"
                font.pixelSize: 18
                font.bold: true
            }
            Item { Layout.fillWidth: true }
            Button {
                text: "Refresh"
                onClicked: {
                    alarmListView.model = smmManager.getAlarmLogsModel()
                }
            }
        }
        ListView {
            id: alarmListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            clip: true
            model: smmManager.getAlarmLogsModel()

            delegate: Rectangle {
                width: alarmListView.width
                height: 50
                color: "#1e293b"
                radius: 12
                border.width: 1
                border.color: model.priority === "Yellow" ? "#eab308" : (model.priority === "Blue" ? "#3b82f6" : "#ef4444")

                Row {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 40

                    Text {
                        text: model.timestamp !== undefined ? model.timestamp : " "
                        color: "#94a3b8"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: "Parameter: " + model.parameter_type
                        color: "#ffffff"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: "Value: " + model.value
                        color: "#f87171"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: "Priority: " + model.priority
                        color: model.priority === "Yellow" ? "#eab308" : (model.priority === "Blue" ? "#3b82f6" : "#ef4444")
                        font.pixelSize: 13
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }
}