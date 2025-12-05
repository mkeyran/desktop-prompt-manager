import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property alias text: textField.text
    property alias placeholderText: textField.placeholderText
    height: 32

    
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 8

        Label {
            text: "🔍"
        }

        TextField {
            id: textField
            Layout.fillWidth: true
            placeholderText: "Search..."
            selectByMouse: true
        }

        Button {
            text: "✕"
            visible: textField.text.length > 0
            flat: true
            onClicked: textField.clear()
           
        }
    }
}