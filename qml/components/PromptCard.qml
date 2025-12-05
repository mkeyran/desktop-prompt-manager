import QtQuick 2.15
import QtQuick.Controls
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
        border.width: 1
        radius: 4
        color: "transparent"
       
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
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }

                Label {
                    visible: root.folderName.length > 0
                    text: root.folderName
                                        
                }
            }

            // Content preview and date
            Label {
                Layout.fillWidth: true
                text: root.content + " • " + Qt.formatDateTime(root.updatedAt, "MMM d")
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }

        // Action buttons - always visible, compact vertical layout
        ColumnLayout {
            spacing: 2
            Layout.preferredWidth: 24

            Button {
                text: "✏"
                flat: true
                font.pointSize: 10
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
                onClicked: {
                    root.editClicked();
                }
                
                ToolTip.visible: hovered
                ToolTip.text: "Edit"
                ToolTip.delay: 500
            }

            Button {
                text: "⋯"
                flat: true
                font.pointSize: 12
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
                onClicked: contextMenu.open()
                
                ToolTip.visible: hovered
                ToolTip.text: "More options"
                ToolTip.delay: 500

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
