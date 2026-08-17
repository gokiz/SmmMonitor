import QtQuick 2.15
import QtQuick.Window 2.15
import Backend 1.0
import Smm.Grafik 1.0
import QtMultimedia

Window {
    width: 600
    height: 650
    visible: true
    title: "SMM UDP Server Dashboard"
    color: "#0f172a"

    MediaPlayer {
        id: serverAlarmPlayer
        source: "qrc:/sounds2/highAlarm.wav"
        loops: MediaPlayer.Infinite
        audioOutput: AudioOutput {volume: 1.0}
    }

    UdpReceiver {
        id: udpReceiver

        onDataReceived: {
            serverGraph.addPoint(udpReceiver.waveform)
            serverGraph.setWaveformSpeed(udpReceiver.waveformSpeed)
        }
        function updateServerAudio() {
            if(udpReceiver.isAlarmMuted) {
                serverAlarmPlayer.stop();
                return;
            }
            if(udpReceiver.isSpo2AlarmActive || udpReceiver.isPulseAlarmActive) {
                let highestPrio = Math.max(
                        udpReceiver.isSpo2AlarmActive ? udpReceiver.spo2AlarmPriority : 0,
                        udpReceiver.isPulseAlarmActive ? udpReceiver.pulseAlarmPriority : 0
                );
                if(highestPrio === 2) serverAlarmPlayer.source = "qrc:/sounds2/highAlarm.wav";
                else if(highestPrio === 1) serverAlarmPlayer.source = "qrc:/sounds2/mediumAlarm.wav";
                else serverAlarmPlayer.source = "qrc:/sounds2/lowAlarm.wav";

                if(serverAlarmPlayer.playbackState !== MediaPlayer.PlayingState) {
                    serverAlarmPlayer.play();
                }
            }else {
                serverAlarmPlayer.stop();
            }
        }
        onAlarmStatusChanged: updateServerAudio()
        onIsAlarmMutedChanged: updateServerAudio()
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
            radius: 8
            border.width: 2
            anchors.horizontalCenter: parent.horizontalCenter

            color: {
                if(udpReceiver.isSpo2AlarmActive ) {
                    if(udpReceiver.spo2AlarmPriority === 2) return "#ef4444";
                    else if(udpReceiver.spo2AlarmPriority === 1) return "#eab308";
                    return "#3b82f6";
                }
                return "#1e293b";
            }
            border.color: udpReceiver.isSpo2AlarmActive ? "#ffffff" : "#10b981"
            SequentialAnimation on opacity {
                            loops: Animation.Infinite
                            running: udpReceiver.isSpo2AlarmActive
                            NumberAnimation { to: 0.4; duration: 500 }
                            NumberAnimation { to: 1.0; duration: 500 }
                        }

            Text {
                anchors.centerIn: parent
                text: "SpO2: " + (udpReceiver.spo2 === 0 ? "--" : udpReceiver.spo2) + " %"
                color: udpReceiver.isSpo2AlarmActive ? "#ffffff" : "#10b981"
                font.pixelSize: 28
                font.bold: true
            }
        }
        Rectangle {
            width: 450
            height: 70
            radius: 8
            border.width: 2
            anchors.horizontalCenter: parent.horizontalCenter
            color: {
                if(udpReceiver.isPulseAlarmActive) {
                    if(udpReceiver.pulseAlarmPriority === 2) return "#ef4444";
                    else if (udpReceiver.pulseAlarmPriority === 1) return "#eab308";
                    return "#3b82f6";
                }
                return "#1e293b";
            }
            border.color: udpReceiver.isPulseAlarmActive ? "#ffffff" : "#10b981"
            SequentialAnimation on opacity {
                loops: Animation.Infinite
                running: udpReceiver.isPulseAlarmActive
                NumberAnimation {to: 0.4; duration: 500}
                NumberAnimation {to: 1.0; duration: 500}
            }

            Text {
                anchors.centerIn: parent
                text: "Pulse: " + (udpReceiver.pulseRate === 0 ? "--" : udpReceiver.pulseRate) + " bpm"
                color: udpReceiver.isPulseAlarmActive ? "#ffffff" : "#10b981"
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
