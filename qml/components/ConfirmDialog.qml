import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root

    property string message: ""
    property string confirmText: "Yes"
    property string cancelText: "No"

    signal confirmed()
    signal cancelled()

    modal: true
    anchors.centerIn: parent
    width: 400

    ColumnLayout {
        anchors.fill: parent
        spacing: 20

        Label {
            Layout.fillWidth: true
            text: root.message
            wrapMode: Text.Wrap
            font.pointSize: 12
        }

        RowLayout {
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: root.cancelText
                onClicked: {
                    root.cancelled()
                    root.close()
                }
            }

            CustomButton {
                text: root.confirmText
                backgroundColor: "#f44336"
                hoverColor: "#d32f2f"
                pressedColor: "#c62828"
                onClicked: {
                    root.confirmed()
                    root.close()
                }
            }
        }
    }
}