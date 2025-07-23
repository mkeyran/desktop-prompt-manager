import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property alias text: textField.text
    property alias placeholderText: textField.placeholderText

    height: 36
    color: "#f8f9fa"
    border.color: textField.activeFocus ? "#007acc" : "#dadce0"
    border.width: 1
    radius: 6

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 8

        Label {
            text: "🔍"
            font.pointSize: 10
            color: "#5f6368"
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
            font.pointSize: 10
            onClicked: textField.clear()
            
            background: Rectangle {
                color: parent.hovered ? "#e8eaed" : "transparent"
                radius: 4
            }
        }
    }
}