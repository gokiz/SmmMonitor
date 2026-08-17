import QtQuick 2.15
import QtQuick.Window 2.15
import Backend 1.0
import Smm.Grafik 1.0

Window {
    width: 600
    height: 650
    visible: true
    title: "SMM UDP Server Dashboard"
    color: "#0f172a"

    UdpReceiver {
        id: udpReceiver

        onDataReceived: {
            serverGraph.addPoint(udpReceiver.waveform)
            serverGraph.setWaveformSpeed(udpReceiver.waveformSpeed)
        }
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        Rectangle {
            width: 14
            height: 14
            radius: 7
            color: udpReceiver.isConnected ? "#10b981" : "#ef4444"
            anchors.verticalCenter: parent.verticalCenter
            Behavior on color {

                ColorAnimation{ duration: 300}
            }
        }
        Text  {
            text: udpReceiver.isConnected ? "Connection is Active (Sensor is Working)" : "Connection Lost / Waiting..."
            color: udpReceiver.isConnected ? "#10b981" : "#ef4444"
            font.pixelSize: 14
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 30

        Text {
            text: "Live Data Stream (UDP)"
            color: "#0ea5e9"
            font.pixelSize: 26
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Rectangle {
            width: 450
            height: 70
            color: "#1e293b"
            radius: 8
            border.color: "#10b981"
            border.width: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                anchors.centerIn: parent
                text: "SpO2: " + (udpReceiver.spo2 === 0 ? "--" : udpReceiver.spo2) + " %"
                color: "#10b981"
                font.pixelSize: 28
                font.bold: true
            }
        }
        Rectangle {
            width: 450
            height: 70
            color: "#1e293b"
            radius: 8
            border.color: "#ef4444"
            border.width: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                anchors.centerIn: parent
                text: "Pulse: " + (udpReceiver.pulseRate === 0 ? "--" : udpReceiver.pulseRate) + " bpm"
                color: "#ef4444"
                font.pixelSize: 28
                font.bold: true
            }
        }
        Rectangle {
            width: parent.width - 40
            height: 250
            color: "#1e293b"
            radius: 8
            border.color: "#eab308"
            border.width: 2
            anchors.horizontalCenter: parent.horizontalCenter

            WaveformGraph {
                id: serverGraph
                anchors.fill: parent
                anchors.margins: 10
                onWidthChanged: {
                    serverGraph.calibrate(Screen.pixelDensity)
                }

                // UYGULAMA İLK AÇILDIĞINDA KALİBRASYONU YAP
                Component.onCompleted: {
                    serverGraph.calibrate(Screen.pixelDensity)
                }
            }
        }
    }
}
