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
                width: 150
                height: 40
                placeholderText: "Choose Date and Time"
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
                width: 150
                height: 40
                placeholderText: "Choose Date and Time"
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
                    text: "Clear the Filter"
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
        Row {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 20
            spacing: 15

            //aralık silme tuşu
            Rectangle{
                width: 130
                height: 40
                color: "#eab308"
                radius: 10
                border.color: "#ca8a04"
                border.width: 1

                Text{
                    text: "Delete Selected"
                    color: "#ffffff"
                    font.bold: true
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if(startDateInput.text !== "-" && endDateInput.text !== "-"){
                            smmManager.deleteHistoryByDateRange(startDateInput.text, endDateInput.text);
                        } else {
                            console.log("Please select a start and end date first.");
                        }
                    }
                }
            }
            //Tümünü sil tuşu
            Rectangle{
                id: deleteAllButton
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
    }
    Popup {
        id: calendarPopup
        width: 440
        height: 320
        modal: true
        anchors.centerIn: parent
        background: Rectangle {color: "#1e293b"; radius: 15; border.color: "#b0e2ff"; border.width: 2}

        property var targetField: null
        property var years: ["2025", "2026", "2027", "2028", "2029", "2030"]
        property var months: ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"]
        property var activeTumbler: yearTumbler

        onOpened: {
            activeTumbler = yearTumbler
            yearTumbler.forceActiveFocus()
        }
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 20

            Text{
                text: "Choose Date & Time"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 18
                Layout.alignment: Qt.AlignHCenter
            }
            //Tarih ve saat çarkları
            RowLayout{
                Layout.alignment: Qt.AlignHCenter
                spacing: 8

                //Yıl çarkı
                Tumbler{
                    id: yearTumbler
                    model:calendarPopup.years
                    currentIndex: 1
                    visibleItemCount: 3
                    Layout.preferredHeight: 120
                    Layout.preferredWidth: 60

                    onActiveFocusChanged: {if (activeFocus) calendarPopup.activeTumbler = yearTumbler }
                    onMovingChanged: {if (moving) {forceActiveFocus(); calendarPopup.activeTumbler = yearTumbler }}

                    Keys.onRightPressed: monthTumbler.forceActiveFocus()
                    Keys.onUpPressed: currentIndex = Math.max(0, currentIndex - 1)
                    Keys.onDownPressed: currentIndex = Math.min(count - 1, currentIndex + 1)

                    WheelHandler {
                        onWheel: (wheel) => {
                                     yearTumbler.forceActiveFocus()
                                     calendarPopup.activeTumbler = yearTumbler
                                     if(wheel.angleDelta.y > 0) yearTumbler.currentIndex = Math.max(0, yearTumbler.currentIndex - 1)
                                     else yearTumbler.currentIndex = Math.min(yearTumbler.count - 1, yearTumbler.currentIndex + 1)

                                 }
                    }
                    delegate: Item {
                        width: yearTumbler.width
                        height: 40
                        Text {
                            anchors.fill: parent
                            text: modelData
                            color: yearTumbler.currentIndex === index ? (calendarPopup.activeTumbler === yearTumbler ? "#4169e1" : "#be185d") : "#473c8b"
                            font.pixelSize: yearTumbler.currentIndex === index ? 22 : 16
                            font.bold: yearTumbler.currentIndex === index
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                yearTumbler.forceActiveFocus()
                                calendarPopup.activeTumbler = yearTumbler
                                yearTumbler.currentIndex = index // Tıklanan rakamı ortaya getirir
                            }
                        }
                    }
                }
                //Ay çarkı
                Tumbler{
                    id: monthTumbler
                    model: calendarPopup.months
                    visibleItemCount: 3
                    Layout.preferredHeight: 120
                    Layout.preferredWidth: 40

                    onActiveFocusChanged: {if (activeFocus) calendarPopup.activeTumbler = monthTumbler }
                    onMovingChanged:  {if(moving) {forceActiveFocus(); calendarPopup.activeTumbler = monthTumbler} }

                    Keys.onLeftPressed: yearTumbler.forceActiveFocus()
                    Keys.onRightPressed: dayTumbler.forceActiveFocus()
                    Keys.onUpPressed: currentIndex = Math.max(0, currentIndex - 1)
                    Keys.onDownPressed: currentIndex = Math.min(count - 1, currentIndex + 1)

                    WheelHandler{
                        onWheel: (wheel) => {
                                     monthTumbler.forceActiveFocus()
                                     calendarPopup.activeTumbler = monthTumbler
                                     if(wheel.angleDelta.y > 0) monthTumbler.currentIndex = Math.max(0, monthTumbler.currentIndex  - 1)
                                     else monthTumbler.currentIndex = Math.min(monthTumbler.count - 1, monthTumbler.currentIndex + 1 )
                                 }
                    }
                    delegate: Item{
                        width: monthTumbler.width
                        height: 40
                        Text{
                            anchors.fill: parent
                            text: modelData
                            color: monthTumbler.currentIndex === index ? (calendarPopup.activeFocus === monthTumbler ? "#4169e1" : "#be185d") : "#473c8b"
                            font.pixelSize: monthTumbler.currentIndex === index ? 22 : 16
                            font.bold: monthTumbler.currentIndex === index
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment:  Text.AlignVCenter
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                monthTumbler.forceActiveFocus()
                                calendarPopup.activeTumbler = monthTumbler
                                yearTumbler.currentIndex = index
                            }
                        }
                    }
                }
                //gün çarkı
                Tumbler{
                    id: dayTumbler
                    model: 31
                    visibleItemCount: 3
                    Layout.preferredHeight: 120
                    Layout.preferredWidth: 40

                    onActiveFocusChanged: { if (activeFocus) calendarPopup.activeTumbler = dayTumbler }
                    onMovingChanged: { if (moving) { forceActiveFocus(); calendarPopup.activeTumbler = dayTumbler } }

                    Keys.onLeftPressed: monthTumbler.forceActiveFocus()
                    Keys.onRightPressed: hourTumbler.forceActiveFocus() // Sağa basınca saate geçer
                    Keys.onUpPressed: currentIndex = Math.max(0, currentIndex - 1)
                    Keys.onDownPressed: currentIndex = Math.min(count - 1, currentIndex + 1)

                    WheelHandler {
                        onWheel: (wheel) => {
                                     dayTumbler.forceActiveFocus()
                                     calendarPopup.activeTumbler = dayTumbler
                                     if (wheel.angleDelta.y > 0) dayTumbler.currentIndex = Math.max(0, dayTumbler.currentIndex - 1)
                                     else dayTumbler.currentIndex = Math.min(dayTumbler.count - 1, dayTumbler.currentIndex + 1)
                                 }
                    }

                    delegate: Item {
                        width: dayTumbler.width
                        height: 40
                        Text {
                            anchors.fill: parent
                            text: (modelData + 1).toString().padStart(2, '0')
                            color: dayTumbler.currentIndex === index ? (calendarPopup.activeTumbler === dayTumbler ? "#4169e1" : "#be185d") : "#473c8b"
                            font.pixelSize: dayTumbler.currentIndex === index ? 22 : 16
                            font.bold: dayTumbler.currentIndex === index
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                dayTumbler.forceActiveFocus();
                                calendarPopup.activeTumbler = dayTumbler;
                                dayTumbler.currentIndex = index }
                        }
                    }
                }
                Item{ Layout.preferredWidth: 20; Layout.preferredHeight: 120 } //görsel ayırcı-tarih ve saat arası boşluk

                //Saat Çarkı
                Tumbler{
                    id:hourTumbler
                    model: 24
                    visibleItemCount: 3
                    Layout.preferredHeight: 120
                    Layout.preferredWidth: 40

                    onActiveFocusChanged: { if (activeFocus) calendarPopup.activeTumbler = hourTumbler }
                    onMovingChanged: { if (moving) { forceActiveFocus(); calendarPopup.activeTumbler = hourTumbler } }
                    Keys.onLeftPressed: dayTumbler.forceActiveFocus()
                    Keys.onRightPressed: minuteTumbler.forceActiveFocus()
                    Keys.onUpPressed: currentIndex.Math.max(0, currentIndex - 1)
                    Keys.onDownPressed: currentIndex = Math.min(count - 1, currentIndex + 1)

                    WheelHandler {
                        onWheel: (wheel) => {
                                     hourTumbler.forceActiveFocus()
                                     calendarPopup.activeTumbler = hourTumbler
                                     if (wheel.angleDelta.y > 0) hourTumbler.currentIndex = Math.max(0, hourTumbler.currentIndex - 1)
                                     else hourTumbler.currentIndex = Math.min(hourTumbler.count - 1, hourTumbler.currentIndex + 1)
                                 }
                    }
                    delegate: Item {
                        width: hourTumbler.width
                        height: 40
                        Text {
                            anchors.fill: parent
                            text: modelData.toString().padStart(2, '0')
                            color: hourTumbler.currentIndex === index ? (calendarPopup.activeTumbler === hourTumbler ? "#4169e1" : "#be185d") : "#473c8b"
                            font.pixelSize: hourTumbler.currentIndex === index ? 22 : 16
                            font.bold: hourTumbler.currentIndex === index
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        MouseArea { anchors.fill: parent;
                            onClicked: {
                                hourTumbler.forceActiveFocus();
                                calendarPopup.activeTumbler = hourTumbler;
                                hourTumbler.currentIndex = index }
                        }
                    }
                }
                //dakika çarkı
                Tumbler {
                    id: minuteTumbler
                    model: 60
                    visibleItemCount: 3
                    Layout.preferredHeight: 120
                    Layout.preferredWidth: 40

                    onActiveFocusChanged: { if (activeFocus) calendarPopup.activeTumbler = minuteTumbler }
                    onMovingChanged: { if (moving) { forceActiveFocus(); calendarPopup.activeTumbler = minuteTumbler } }

                    Keys.onLeftPressed: hourTumbler.forceActiveFocus()
                    Keys.onUpPressed: currentIndex = Math.max(0, currentIndex - 1)
                    Keys.onDownPressed: currentIndex = Math.min(count - 1, currentIndex + 1)

                    WheelHandler {
                        onWheel: (wheel) => {
                                     minuteTumbler.forceActiveFocus()
                                     calendarPopup.activeTumbler = minuteTumbler
                                     if (wheel.angleDelta.y > 0) minuteTumbler.currentIndex = Math.max(0, minuteTumbler.currentIndex - 1)
                                     else minuteTumbler.currentIndex = Math.min(minuteTumbler.count - 1, minuteTumbler.currentIndex + 1)
                                 }
                    }

                    delegate: Item {
                        width: minuteTumbler.width
                        height: 40
                        Text {
                            anchors.fill: parent
                            text: modelData.toString().padStart(2, '0')
                            color: minuteTumbler.currentIndex === index ? (calendarPopup.activeTumbler === minuteTumbler ? "#4169e1" : "#be185d") : "#473c8b"
                            font.pixelSize: minuteTumbler.currentIndex === index ? 22 : 16
                            font.bold: minuteTumbler.currentIndex === index
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        MouseArea { anchors.fill: parent;
                            onClicked: {
                                minuteTumbler.forceActiveFocus();
                                calendarPopup.activeTumbler = minuteTumbler;
                                minuteTumbler.currentIndex = index }
                        }
                    }
                }
            }
            //onaylama butonu
            Rectangle{
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                color: "#ec4899"
                radius: 10

                Text{
                    text: "Apply"
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 16
                    anchors.centerIn: parent
                }
                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if(calendarPopup.targetField) {
                            let y = calendarPopup.years[yearTumbler.currentIndex]
                            let m = calendarPopup.months[monthTumbler.currentIndex]
                            let d = (dayTumbler.currentIndex + 1).toString().padStart(2, '0')
                            let h = hourTumbler.currentIndex.toString().padStart(2, '0')
                            let min = minuteTumbler.currentIndex.toString().padStart(2, '0')

                            calendarPopup.targetField.text  = y + "-" + m + "-" + d + " " + h + ":" + min

                        }
                        calendarPopup.close()
                    }
                }
            }
        }
    }
}