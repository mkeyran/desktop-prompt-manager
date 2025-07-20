import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property alias text: textField.text
    property alias placeholderText: textField.placeholderText

    height: 40
    color: "white"
    border.color: textField.activeFocus ? "#2196f3" : "#e0e0e0"
    border.width: textField.activeFocus ? 2 : 1
    radius: 20

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        spacing: 10

        Label {
            text: "🔍"
            font.pointSize: 12
        }

        TextField {
            id: textField
            Layout.fillWidth: true
            placeholderText: "Search..."
            background: Rectangle {
                color: "transparent"
            }
            selectByMouse: true
        }

        Button {
            text: "✕"
            visible: textField.text.length > 0
            flat: true
            onClicked: textField.clear()
            
            background: Rectangle {
                color: parent.hovered ? "#f0f0f0" : "transparent"
                radius: 12
            }
        }
    }
}