import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("SMM Modülü SpO2 Monitörü")
    color: "#1e1e1e"

    //verilerin gösterileceği Rectangle
    Rectangle {
        id: infoRectangle
        width: 300
        height: 200
        color: "#2d2d2d"
        border.color: "#00ffcc"
        border.width: 3
        radius: 15
        anchors.centerIn: parent

        Column {
            anchors.centerIn: parent
            spacing: 15

            Text {
                text: "SATURASYON (SpO2"
                color: "#aaaaaa"
                font.pixelSize: 18
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                //C++ sınıfındaki 'saturation' property'sini dinler
                //değer her değiştiğinde bu yazı otomatik güncellenir
                text: smmManager.saturation == 0 ? "--" : "%" + smmManager.saturation
                color: smmManager.saturation < 90 ? "#ff3333" : "#00ffcc" // düşük saturasyonda kırmızı alam rengi
                font.pixelSize: 64
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
