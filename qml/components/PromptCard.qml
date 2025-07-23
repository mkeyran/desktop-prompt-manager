import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import PromptManager 1.0

Item {
    id: root

    property int promptId: 0
    property string title: ""
    property string content: ""
    property string folderName: ""
    property date updatedAt: new Date()

    PlaceholderUtils {
        id: placeholderUtils
    }

    signal editClicked
    signal deleteClicked
    signal duplicateClicked
    signal fillPlaceholdersClicked
    signal fillPlaceholdersWithContentClicked(string content)

    height: 64
    
    Rectangle {
        anchors.fill: parent
        color: mouseArea.containsMouse ? "#f1f3f4" : "#fafbfc"
        border.color: "#e8eaed"
        border.width: 1
        radius: 4
        
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: "#e8eaed"
            visible: false
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton
        onClicked: {
            // Always go to placeholder filling when clicking a prompt
            root.fillPlaceholdersWithContentClicked(root.content);
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // Content column
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            // Title and folder
            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: root.title || "Untitled"
                    font.pointSize: 11
                    color: "#202124"
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }

                Label {
                    visible: root.folderName.length > 0
                    text: root.folderName
                    font.pointSize: 9
                    color: "#5f6368"
                    
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -4
                        color: "#f1f3f4"
                        radius: 3
                        z: -1
                    }
                }
            }

            // Content preview and date
            Label {
                Layout.fillWidth: true
                text: root.content + " • " + Qt.formatDateTime(root.updatedAt, "MMM d")
                color: "#5f6368"
                font.pointSize: 9
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }

        // Action buttons
        RowLayout {
            spacing: 4

            Button {
                text: "Edit"
                flat: true
                font.pointSize: 9
                visible: mouseArea.containsMouse
                onClicked: {
                    root.editClicked();
                }
            }

            Button {
                text: "⋯"
                flat: true
                font.pointSize: 12
                visible: mouseArea.containsMouse
                onClicked: contextMenu.open()

                Menu {
                    id: contextMenu
                    MenuItem {
                        text: "Fill Placeholders"
                        onTriggered: root.fillPlaceholdersWithContentClicked(root.content)
                    }
                    MenuItem {
                        text: "Duplicate"
                        onTriggered: root.duplicateClicked()
                    }
                    MenuSeparator {}
                    MenuItem {
                        text: "Delete"
                        onTriggered: confirmDeleteDialog.open()
                    }
                }
            }
        }
    }

    // Delete confirmation dialog
    Dialog {
        id: confirmDeleteDialog
        anchors.centerIn: parent
        title: "Delete Prompt"
        modal: true

        Label {
            text: `Are you sure you want to delete "${root.title}"?`
        }

        standardButtons: Dialog.Yes | Dialog.No

        onAccepted: root.deleteClicked()
    }
}
