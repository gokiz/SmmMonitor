import QtQuick 2.15
import QtQuick.Window 2.15
import Backend 1.0

Window {
    width: 400
    height: 350
    visible: true
    title: "SMM UDP Server Dashboard"
    color: "#0f172a"

    UdpReceiver {
        id: udpReceiver
    }
    Column {
        anchors.centerIn: parent
        spacing: 25

        Text {
            text: "Live Data Stream (UDP)"
            color: "#0ea5e9"
            font.pixelSize: 20
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Rectangle {
            width: 250
            height: 60
            color: "#1e293b"
            radius: 8
            border.color: "#10b981"

            Text {
                anchors.centerIn: parent
                text: "SpO2: " + (udpReceiver.spo2 === 0 ? "--" : udpReceiver.spo2) + "%"
                color: "#10b981"
                font.pixelSize: 24
                font.bold: true
            }
        }
        Rectangle {
            width: 250
            height: 60
            color: "#1e293b"
            radius: 8
            border.color: "#ef4444"

            Text {
                anchors.centerIn: parent
                text: "Pulse: " + (udpReceiver.pulseRate === 0 ? "--" : udpReceiver.pulseRate) + "bpm"
                color: "#ef4444"
                font.pixelSize: 24
                font.bold: true
            }
        }
        Rectangle {
            width: 250
            height: 60
            color: "#1e293b"
            radius: 8
            border.color: "#eab308"

            Text {
                anchors.centerIn: parent
                text: "Waveform Values: " + udpReceiver.waveform
                color: "#eab308"
                font.pixelSize: 24
                font.bold: true
            }
        }
    }
}
