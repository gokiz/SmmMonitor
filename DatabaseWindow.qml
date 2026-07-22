import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

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

        //sola dayalı filtreleme alanı:
        Row{
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 20
            spacing: 15

            TextField {
                id: startDateInput
                width: 120
                height: 40
                placeholderText: "Choose Date"
                font.pixelSize: 14
                readOnly: true

                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        calendarPopup.targetField = startDateInput
                        calendarPopup.open();
                    }
                }
            }
            Text {
                text: "-"
                color: "#ffffff"
                font.pixelSize: 14
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
            TextField{
                id: endDateInput
                width: 120
                height: 40
                placeholderText: "Choose Date"
                font.pixelSize: 14
                readOnly: true //klavye açılmasını engeller

                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        calendarPopup.targetField = endDateInput
                        calendarPopup.open()
                    }
                }
            }
            //filtreleme butonu
            Rectangle {
                width: 90
                height: 40
                color: "#4ff9c1" //yesil
                radius: 10
                Text{
                    text: "Filter"
                    color: "#ffffff"
                    font.bold: true
                    anchors.centerIn: parent
                }
                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        smmManager.filterHistoryByDate(startDateInput.text, endDateInput.text)
                    }
                }
            }
            //temizleme butonu
            Rectangle{
                width: 90
                height: 40
                color: "#fc3f3f" //kırmızı
                radius: 10
                Text{
                    text: "Clear"
                    color:  "#ffffff"
                    font.bold: true
                    anchors.centerIn: parent
                }
                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        startDateInput.text = "-"
                        endDateInput.text = "-"
                        smmManager.clearFilter()
                    }
                }
            }
        }
        //back tuşu
        Rectangle {
            id:backButton
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 35
            width: 100
            height: 40
            color: "#473c8b"
            radius: 10
            border.color: "#b0e2ff"
            border.width:1

            Text{
                text: "Back"
                color: "#ffffff"
                font.pixelSize: 14
                font.bold: true
                anchors.centerIn: parent
            }
            MouseArea {
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
        anchors.bottom: footerArea.top
        anchors.right: parent.right
        anchors.left: parent.left

        anchors.margins: 20
        anchors.topMargin: 10
        spacing: 8
        clip: true


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
                    text: model.timestamp !== undefined ? model.timestamp.replace("T", " ") : " " //veritabanından gelen zamanlayıcı
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
    Item{
        id: footerArea
        width: parent.width
        height: 70
        anchors.bottom: parent.bottom
        // silme tuşu
        Rectangle {
            id: deleteAllButton
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 20


            width: 100
            height: 40
            color: "#991b1b"
            radius: 10
            border.color: "#ef4444"
            border.width: 1


            Text{
                text: "Delete All"
                color: "#ffffff"
                font.bold: true
                anchors.centerIn: parent
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    smmManager.clearHistory();
                }
            }
        }
    }
    Popup {
        id: calendarPopup
        width: 300
        height: 320
        modal: true
        anchors.centerIn: parent
        background: Rectangle { color: "#1e293b"; radius: 15; border.color: "#b0e2ff"; border.width: 2 }

        property var targetField: null
        property var years: ["2025", "2026", "2027", "2028", "2029", "2030"]
        property var months: ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"]

        // RENK DEĞİŞİMİNİ GARANTİLEMEK İÇİN AKTİF ÇARKI HAFIZADA TUTUYORUZ
        property var activeTumbler: yearTumbler

        onOpened: {
            activeTumbler = yearTumbler
            yearTumbler.forceActiveFocus()
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 20

            Text {
                text: "Choose Date"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 18
                Layout.alignment: Qt.AlignHCenter
            }

            // --- TARİH ÇARKLARI (Yıl - Ay - Gün) ---
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 15

                // 1. YIL ÇARKI
                Tumbler {
                    id: yearTumbler
                    model: calendarPopup.years
                    currentIndex: 1
                    visibleItemCount: 3
                    Layout.preferredHeight: 120

                    // Odaklandığında kendini aktif çark olarak belirler
                    onActiveFocusChanged: { if (activeFocus) calendarPopup.activeTumbler = yearTumbler }
                    onMovingChanged: { if (moving) forceActiveFocus() }

                    Keys.onRightPressed: monthTumbler.forceActiveFocus()
                    Keys.onUpPressed: currentIndex = Math.max(0, currentIndex - 1)
                    Keys.onDownPressed: currentIndex = Math.min(count - 1, currentIndex + 1)

                    WheelHandler {
                        onWheel: (wheel) => {
                                     yearTumbler.forceActiveFocus()
                                     if(wheel.angleDelta.y > 0) currentIndex = Math.max(0, currentIndex - 1)
                                     else currentIndex = Math.min(count - 1, currentIndex + 1)
                                 }
                    }

                    delegate: Text {
                        text: modelData
                        // Rengi artık doğrudan bizim manuel değişkenimize (activeTumbler) göre alır
                        color: yearTumbler.currentIndex === index ? (calendarPopup.activeTumbler === yearTumbler ? "#ec4899" : "#aaaaff") : "#473c8b"
                        font.pixelSize: yearTumbler.currentIndex === index ? 22 : 16
                        font.bold: yearTumbler.currentIndex === index
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                // 2. AY ÇARKI
                Tumbler {
                    id: monthTumbler
                    model: calendarPopup.months
                    visibleItemCount: 3
                    Layout.preferredHeight: 120

                    onActiveFocusChanged: { if (activeFocus) calendarPopup.activeTumbler = monthTumbler }
                    onMovingChanged: { if (moving) forceActiveFocus() }

                    Keys.onLeftPressed: yearTumbler.forceActiveFocus()
                    Keys.onRightPressed: dayTumbler.forceActiveFocus()
                    Keys.onUpPressed: currentIndex = Math.max(0, currentIndex - 1)
                    Keys.onDownPressed: currentIndex = Math.min(count - 1, currentIndex + 1)

                    WheelHandler {
                        onWheel: (wheel) => {
                                     monthTumbler.forceActiveFocus()
                                     if(wheel.angleDelta.y > 0) currentIndex = Math.max(0, currentIndex - 1)
                                     else currentIndex = Math.min(count - 1, currentIndex + 1)
                                 }
                    }

                    delegate: Text {
                        text: modelData
                        color: monthTumbler.currentIndex === index ? (calendarPopup.activeTumbler === monthTumbler ? "#ec4899" : "#aaaaff") : "#473c8b"
                        font.pixelSize: monthTumbler.currentIndex === index ? 22 : 16
                        font.bold: monthTumbler.currentIndex === index
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                // 3. GÜN ÇARKI
                Tumbler {
                    id: dayTumbler
                    model: 31
                    visibleItemCount: 3
                    Layout.preferredHeight: 120

                    onActiveFocusChanged: { if (activeFocus) calendarPopup.activeTumbler = dayTumbler }
                    onMovingChanged: { if (moving) forceActiveFocus() }

                    Keys.onLeftPressed: monthTumbler.forceActiveFocus()
                    Keys.onUpPressed: currentIndex = Math.max(0, currentIndex - 1)
                    Keys.onDownPressed: currentIndex = Math.min(count - 1, currentIndex + 1)

                    WheelHandler {
                        onWheel: (wheel) => {
                                     dayTumbler.forceActiveFocus()
                                     if (wheel.angleDelta.y > 0) currentIndex = Math.max(0, currentIndex - 1)
                                     else currentIndex = Math.min(count - 1, currentIndex + 1)
                                 }
                    }

                    delegate: Text {
                        text: (modelData + 1).toString().padStart(2, '0')
                        color: dayTumbler.currentIndex === index ? (calendarPopup.activeTumbler === dayTumbler ? "#ec4899" : "#aaaaff") : "#473c8b"
                        font.pixelSize: dayTumbler.currentIndex === index ? 22 : 16
                        font.bold: dayTumbler.currentIndex === index
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            // --- ONAYLA BUTONU ---
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                color: "#ec4899"
                radius: 10

                Text {
                    text: "Apply"
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 16
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if(calendarPopup.targetField) {
                            let y = calendarPopup.years[yearTumbler.currentIndex]
                            let m = calendarPopup.months[monthTumbler.currentIndex]
                            let d = (dayTumbler.currentIndex + 1).toString().padStart(2, '0')

                            calendarPopup.targetField.text = y + "-" + m + "-" + d
                        }
                        calendarPopup.close()
                    }
                }
            }
        }
    }
}