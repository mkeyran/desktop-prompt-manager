import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts 1.15
import "../components"

Page {
    id: root

    property int promptId: -1
    property string content: ""

    signal finished
    signal back

    Component.onCompleted: {
        console.log("PlaceholderFillingScreen - promptId:", promptId, "content:", content);
        if (content.length > 0) {
            // Use provided content (preferred method)
            console.log("Initializing with content:", content);
            placeholderViewModel.initialize(content);
        } else if (promptId > 0) {
            // Fallback: Load prompt content from ID through the ViewModel
            let prompt = promptListViewModel.getPromptById(promptId);
            if (prompt) {
                console.log("Initializing with prompt content:", prompt.content);
                placeholderViewModel.initialize(prompt.content);
            }
        }
    }

    header: ToolBar {
        height: 70
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10

            Button {
                text: "←"
                onClicked: root.back()
            }

            Label {
                text: "Fill Placeholders"
                font.bold: true
                Layout.fillWidth: true
            }

            Label {
                text: `${placeholderViewModel.placeholders.length} placeholders`
                visible: placeholderViewModel.placeholders.length > 0
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // All placeholder inputs
        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(500, placeholderViewModel.placeholders.length * 45)
            visible: placeholderViewModel.placeholders.length > 0
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: 15

                Repeater {
                    model: placeholderViewModel.placeholders

                    delegate: RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredWidth: parent.width
                        spacing: 10

                        ColumnLayout {
                            Layout.preferredWidth: 120
                            Layout.minimumWidth: 80
                            spacing: 2

                            Label {
                                text: modelData
                                font.bold: !placeholderViewModel.hasDefaultValue(modelData)
                            }

                            Label {
                                text: "(optional)"
                                color: "#666"
                                visible: placeholderViewModel.hasDefaultValue(modelData)
                            }
                        }

                        TextField {
                            Layout.fillWidth: true
                            placeholderText: placeholderViewModel.hasDefaultValue(modelData) ? `Optional (default: ${placeholderViewModel.getDefaultValue(modelData)})` : `Enter value for ${modelData}...`
                            text: placeholderViewModel.getAllValues()[modelData] || ""
                            onTextChanged: {
                                let values = placeholderViewModel.getAllValues();
                                values[modelData] = text;
                                // Update the view model with the new value for this placeholder
                                placeholderViewModel.currentIndex = placeholderViewModel.placeholders.indexOf(modelData);
                                placeholderViewModel.currentValue = text;
                                placeholderViewModel.saveCurrentValue();
                            }
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
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: "Copy to Clipboard"
                    visible: placeholderViewModel.isComplete
                    onClicked: {
                        clipboardUtils.copyToClipboard(placeholderViewModel.processedContent);
                        console.log("Copied to clipboard:", placeholderViewModel.processedContent.length, "characters");
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
                text: placeholderViewModel.isComplete ? "✓ All placeholders filled!" : `${Object.keys(placeholderViewModel.getAllValues()).length} / ${placeholderViewModel.placeholders.length} placeholders filled`
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
                color: "#666"
            }
        }
    }
}
