import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"

Page {
    id: root

    property int promptId: -1
    property string content: ""

    signal finished()
    signal back()

    Component.onCompleted: {
        console.log("PlaceholderFillingScreen - promptId:", promptId, "content:", content)
        if (content.length > 0) {
            // Use provided content (preferred method)
            console.log("Initializing with content:", content)
            placeholderViewModel.initialize(content)
        } else if (promptId > 0) {
            // Fallback: Load prompt content from ID through the ViewModel
            let prompt = promptListViewModel.getPromptById(promptId)
            if (prompt) {
                console.log("Initializing with prompt content:", prompt.content)
                placeholderViewModel.initialize(prompt.content)
            }
        }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10

            Button {
                text: "←"
                onClicked: root.back()
            }

            Label {
                text: "Fill Placeholders"
                font.pointSize: 16
                font.bold: true
                Layout.fillWidth: true
            }

            Label {
                text: `${placeholderViewModel.currentIndex + 1} / ${placeholderViewModel.placeholders.length}`
                visible: placeholderViewModel.placeholders.length > 0
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // Current placeholder input
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10
            visible: placeholderViewModel.placeholders.length > 0

            Label {
                text: `Enter value for: ${placeholderViewModel.currentPlaceholder}`
                font.bold: true
                font.pointSize: 14
            }

            TextField {
                id: placeholderInput
                Layout.fillWidth: true
                placeholderText: `Value for ${placeholderViewModel.currentPlaceholder}...`
                text: placeholderViewModel.currentValue
                onTextChanged: placeholderViewModel.currentValue = text
                onAccepted: {
                    if (!placeholderViewModel.goNext()) {
                        // Last placeholder, save and show results
                        placeholderViewModel.saveCurrentValue()
                    }
                }

                Keys.onPressed: function(event) {
                    if (event.key === Qt.Key_Tab && !event.modifiers) {
                        event.accepted = true
                        if (!placeholderViewModel.goNext()) {
                            placeholderViewModel.saveCurrentValue()
                        }
                    }
                }
            }

            // Navigation buttons
            RowLayout {
                Layout.fillWidth: true

                Button {
                    text: "← Previous"
                    enabled: placeholderViewModel.canGoPrevious
                    onClicked: placeholderViewModel.goPrevious()
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: placeholderViewModel.canGoNext ? "Next →" : "Finish"
                    onClicked: {
                        if (placeholderViewModel.canGoNext) {
                            placeholderViewModel.goNext()
                        } else {
                            placeholderViewModel.saveCurrentValue()
                        }
                    }
                }
            }
        }

        // Preview section
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            RowLayout {
                Label {
                    text: "Preview"
                    font.bold: true
                    font.pointSize: 14
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: "Copy to Clipboard"
                    visible: placeholderViewModel.isComplete
                    onClicked: {
                        clipboardUtils.copyToClipboard(placeholderViewModel.processedContent)
                        console.log("Copied to clipboard:", placeholderViewModel.processedContent.length, "characters")
                    }
                }

                Button {
                    text: "Reset"
                    onClicked: placeholderViewModel.reset()
                }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                TextArea {
                    id: previewArea
                    text: placeholderViewModel.processedContent
                    readOnly: true
                    wrapMode: TextArea.Wrap
                    selectByMouse: true

                    background: Rectangle {
                        color: placeholderViewModel.isComplete ? "#f0f8ff" : "#f8f8f8"
                        border.color: placeholderViewModel.isComplete ? "#4CAF50" : "#ddd"
                        border.width: 1
                        radius: 4
                    }
                }
            }
        }

        // Completion status
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: placeholderViewModel.isComplete ? "#4CAF50" : "#FFC107"
            radius: 4
            visible: placeholderViewModel.placeholders.length > 0

            Label {
                anchors.centerIn: parent
                text: placeholderViewModel.isComplete ? 
                      "✓ All placeholders filled!" : 
                      `${Object.keys(placeholderViewModel.getAllValues()).length} / ${placeholderViewModel.placeholders.length} placeholders filled`
                color: "white"
                font.bold: true
            }
        }

        // No placeholders message
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: placeholderViewModel.placeholders.length === 0
            color: "transparent"

            Label {
                anchors.centerIn: parent
                text: "No placeholders found in this content."
                font.pointSize: 16
                color: "#666"
            }
        }
    }
}