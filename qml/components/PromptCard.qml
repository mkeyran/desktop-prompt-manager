import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import PromptManager 1.0

Rectangle {
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

    height: 120
    color: mouseArea.containsMouse ? "#f5f5f5" : "white"
    border.color: "#e0e0e0"
    border.width: 1
    radius: 8

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
        anchors.margins: 15
        spacing: 15

        // Content column
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            // Title and folder
            RowLayout {
                Layout.fillWidth: true

                Label {
                    text: root.title || "Untitled"
                    font.bold: true
                    font.pointSize: 12
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }

                Rectangle {
                    visible: root.folderName.length > 0
                    color: "#e3f2fd"
                    border.color: "#2196f3"
                    border.width: 1
                    radius: 12
                    height: 24
                    width: folderLabel.implicitWidth + 16

                    Label {
                        id: folderLabel
                        text: root.folderName
                        anchors.centerIn: parent
                        font.pointSize: 9
                        color: "#1976d2"
                    }
                }
            }

            // Content preview
            Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: root.content
                color: "#666"
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                maximumLineCount: 2
            }

            // Date
            Label {
                text: Qt.formatDateTime(root.updatedAt, "MMM d, yyyy hh:mm")
                color: "#999"
                font.pointSize: 9
            }
        }

        // Action buttons
        ColumnLayout {
            spacing: 5

            Button {
                text: "Edit"
                onClicked: {
                    root.editClicked();
                }
            }

            Button {
                text: "⋯"
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
