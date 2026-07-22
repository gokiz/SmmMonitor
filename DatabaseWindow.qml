import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id:dbWindow
    width: 800
    height: 680
    title: "Historical Measurement Records"
    color:"#0f172a"

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

    Item {
        id:headerArea
        width:  parent.width
        height: 70
        anchors.top: parent.top

        Rectangle{
            id: backButton
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 35
            width: 100
            height: 40
            color: "#473c8b"
            radius: 10
            border.color: "#b0e2ff"
            border.width: 1


            Text{
                text:"Back"
                color:"#ffffff"
                font.pixelSize: 14
                font.bold: true
                anchors.centerIn: parent
            }
            MouseArea{
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    dbWindow.hide()
                }
            }

        }
    }

    ListView {
        id: historyList
        anchors.top: headerArea.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left

        anchors.margins: 20
        anchors.topMargin: 10
        spacing: 8
        clip: true
        anchors.horizontalCenter: parent.horizontalCenter

        model: smmManager.getHistoryModel()


        delegate: Rectangle{
            width: historyList.width
            height: 45
            color: "#8b658b"
            radius: 20
            border.width: 1
            border.color: "#334155"

            Row{
                anchors.fill: parent
                anchors.margins: 12
                spacing: 50

                Text {
                    text: model.timestamp !== undefined ? model.timestamp : " " //veritabanından gelen zamanlayıcı
                    color: "#473c8b"
                    font.pixelSize: 14
                    font.bold: true
                }
                Text{
                    text: "SpO2: %" + model.spo2 //veritabanından gelen spo2
                    color: dbWindow.getSpo2Color(model.spo2 !== undefined ? model.spo2 : "--")
                    font.pixelSize: 14
                    font.bold: true
                }
                Text {
                    text:"Pulse: " + model.pulseRate + " bpm" //veritabanından gelen pulse rate
                    color: dbWindow.getPulseColor(model.pulseRate !== undefined ? model.pulseRate : "--")
                    font.pixelSize: 14
                    font.bold: true
                }
            }
        }
    }
}