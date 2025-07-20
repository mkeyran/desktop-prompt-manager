import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"

Page {
    id: root

    signal editPrompt(int promptId)
    signal createNewPrompt()
    signal fillPlaceholders(int promptId)
    signal fillPlaceholdersWithContent(int promptId, string content)

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10

            Label {
                text: "Prompt Manager"
                font.pointSize: 18
                font.bold: true
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: "New Prompt"
                onClicked: root.createNewPrompt()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // Search bar
        SearchBar {
            id: searchBar
            Layout.fillWidth: true
            placeholderText: "Search prompts..."
            text: promptListViewModel.searchText
            onTextChanged: promptListViewModel.searchText = text
        }

        // Folder chips
        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            clip: true

            Row {
                spacing: 10
                padding: 5

                FolderChip {
                    text: "All"
                    selected: promptListViewModel.selectedFolderId === -1
                    onClicked: promptListViewModel.selectedFolderId = -1
                }

                FolderChip {
                    text: "Uncategorized"
                    selected: promptListViewModel.selectedFolderId === 0
                    onClicked: promptListViewModel.selectedFolderId = 0
                }

                Repeater {
                    model: promptListViewModel.folders

                    FolderChip {
                        text: modelData.name
                        selected: promptListViewModel.selectedFolderId === modelData.id
                        onClicked: promptListViewModel.selectedFolderId = modelData.id
                    }
                }
            }
        }

        // Prompts list
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: promptsList
                model: promptListViewModel
                spacing: 10

                delegate: PromptCard {
                    width: promptsList.width
                    promptId: model.id
                    title: model.title
                    content: model.content
                    folderName: model.folderName || ""
                    updatedAt: model.updatedAt

                    onEditClicked: root.editPrompt(promptId)
                    onDeleteClicked: promptListViewModel.deletePrompt(promptId)
                    onDuplicateClicked: promptListViewModel.duplicatePrompt(promptId)
                    onFillPlaceholdersClicked: root.fillPlaceholdersWithContent(promptId, model.content)
                    onFillPlaceholdersWithContentClicked: function(content) {
                        root.fillPlaceholdersWithContent(promptId, content)
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    visible: promptsList.count === 0 && !promptListViewModel.isLoading
                    color: "transparent"

                    Label {
                        anchors.centerIn: parent
                        text: promptListViewModel.searchText.length > 0 ? 
                              "No prompts found for your search" : 
                              "No prompts yet. Create your first prompt!"
                        color: "#666"
                        font.pointSize: 16
                    }
                }
            }
        }

        // Loading indicator
        BusyIndicator {
            Layout.alignment: Qt.AlignHCenter
            visible: promptListViewModel.isLoading
            running: visible
        }

        // Error message
        Label {
            Layout.fillWidth: true
            text: promptListViewModel.errorMessage
            color: "red"
            visible: promptListViewModel.errorMessage.length > 0
            wrapMode: Text.Wrap
        }
    }
}