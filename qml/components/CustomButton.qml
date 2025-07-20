import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: root

    property color backgroundColor: "#2196f3"
    property color hoverColor: "#1976d2"
    property color pressedColor: "#1565c0"

    background: Rectangle {
        color: root.pressed ? root.pressedColor : 
               (root.hovered ? root.hoverColor : root.backgroundColor)
        radius: 4
        
        Behavior on color {
            ColorAnimation {
                duration: 150
            }
        }
    }

    contentItem: Label {
        text: root.text
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.bold: true
    }
}