import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root

    property string text: ""
    property bool selected: false

    signal clicked()

    height: 32
    width: label.implicitWidth + 20
    color: selected ? "#2196f3" : (mouseArea.containsMouse ? "#f0f0f0" : "white")
    border.color: selected ? "#1976d2" : "#e0e0e0"
    border.width: 1
    radius: 16

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
        color: root.selected ? "white" : "#333"
        font.pointSize: 10
    }
}