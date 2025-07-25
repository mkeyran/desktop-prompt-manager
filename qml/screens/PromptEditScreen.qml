import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"

Page {
    id: root

    property int promptId: -1
    property bool isEditing: false

    signal saved
    signal cancelled
    signal fillPlaceholders(string content)

    Component.onCompleted: {
        if (promptId > 0) {
            promptEditViewModel.loadPrompt(promptId);
        } else {
            promptEditViewModel.createNewPrompt();
        }
    }

    header: ToolBar {
        height: 70
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10

            Button {
                text: "←"
                onClicked: root.cancelled()
            }

            Label {
                text: root.isEditing ? "Edit Prompt" : "New Prompt"
                font.pointSize: 16
                font.bold: true
                Layout.fillWidth: true
            }

            Button {
                text: "Save"
                enabled: promptEditViewModel.canSave
                onClicked: {
                    if (promptEditViewModel.savePrompt()) {
                        root.saved();
                    }
                }
            }
        }
    }

    ColumnLayout {
        width: parent.width
        spacing: 20
        anchors.fill: parent
        anchors.margins: 20

        // Title field
        ColumnLayout {
            Layout.fillWidth: true
            // Layout.preferredHeight: 50
            spacing: 5

            Label {
                text: "Title"
                font.bold: true
            }

            TextField {
                id: titleField
                Layout.fillWidth: true
                placeholderText: "Enter prompt title..."
                text: promptEditViewModel.title
                onTextChanged: {
                    promptEditViewModel.title = text;
                }
                selectByMouse: true
            }
        }

        // Folder selection
        ColumnLayout {
            Layout.fillWidth: true
            // Layout.preferredHeight: 100
            spacing: 5

            RowLayout {
                Label {
                    text: "Folder"
                    font.bold: true
                }

                Button {
                    text: "New Folder"
                    onClicked: newFolderDialog.open()
                }
            }

            ComboBox {
                Layout.fillWidth: true
                model: ["No folder"].concat(promptEditViewModel.folders.map(f => f.name))
                currentIndex: {
                    if (promptEditViewModel.selectedFolderId <= 0)
                        return 0;
                    for (let i = 0; i < promptEditViewModel.folders.length; i++) {
                        if (promptEditViewModel.folders[i].id === promptEditViewModel.selectedFolderId) {
                            return i + 1;
                        }
                    }
                    return 0;
                }
                onCurrentIndexChanged: {
                    if (currentIndex === 0) {
                        promptEditViewModel.selectedFolderId = -1;
                    } else {
                        promptEditViewModel.selectedFolderId = promptEditViewModel.folders[currentIndex - 1].id;
                    }
                }
            }
        }

        // Content field
        ColumnLayout {
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 5

            RowLayout {
                Label {
                    text: "Content"
                    font.bold: true
                }

                Label {
                    text: promptEditViewModel.hasPlaceholders ? "• Contains placeholders" : ""
                    color: "#0066cc"
                    font.italic: true
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: "Fill Placeholders"
                    visible: promptEditViewModel.hasPlaceholders
                    onClicked: root.fillPlaceholders(promptEditViewModel.content)
                }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                TextArea {
                    id: contentArea
                    placeholderText: "Enter your prompt content...\n\nYou can use {{placeholder}} syntax for dynamic content."
                    text: promptEditViewModel.content
                    onTextChanged: {
                        promptEditViewModel.content = text;
                    }
                    wrapMode: TextArea.Wrap
                    selectByMouse: true

                    background: Rectangle {
                        color: "white"
                        border.color: contentArea.activeFocus ? "#2196f3" : "#e0e0e0"
                        border.width: contentArea.activeFocus ? 2 : 1
                        radius: 4
                    }
                }
            }
        }

        // Loading indicator
        BusyIndicator {
            Layout.alignment: Qt.AlignHCenter
            visible: promptEditViewModel.isLoading
            running: visible
        }

        // Error message
        Label {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            text: promptEditViewModel.errorMessage
            color: "red"
            visible: promptEditViewModel.errorMessage.length > 0
            wrapMode: Text.Wrap
        }
    }

    // New folder dialog
    Dialog {
        id: newFolderDialog
        anchors.centerIn: parent
        width: 400
        title: "Create New Folder"
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: "Folder Name:"
            }

            TextField {
                id: folderNameField
                Layout.fillWidth: true
                placeholderText: "Enter folder name..."
                onAccepted: {
                    if (text.trim().length > 0) {
                        if (promptEditViewModel.createFolder(text.trim())) {
                            newFolderDialog.close();
                            folderNameField.clear();
                        }
                    }
                }
            }
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            if (folderNameField.text.trim().length > 0) {
                if (promptEditViewModel.createFolder(folderNameField.text.trim())) {
                    folderNameField.clear();
                }
            }
        }

        onRejected: {
            folderNameField.clear();
        }
    }
}
