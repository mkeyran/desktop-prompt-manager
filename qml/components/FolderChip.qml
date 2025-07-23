import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root

    property string text: ""
    property bool selected: false

    signal clicked()

    height: 24
    width: label.implicitWidth + 12
    color: selected ? "#007acc" : (mouseArea.containsMouse ? "#e8eaed" : "transparent")
    border.color: "transparent"
    border.width: 0
    radius: 4

    Behavior on color {
        ColorAnimation {
            duration: 150
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }

    Label {
        id: label
        text: root.text
        anchors.centerIn: parent
        color: root.selected ? "white" : "#3c4043"
        font.pointSize: 9
        font.weight: root.selected ? Font.Medium : Font.Normal
    }
}