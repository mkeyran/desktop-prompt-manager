import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import PromptManager 1.0

ApplicationWindow {
    id: window
    width: 1200
    height: 800
    visible: true
    title: "Prompt Manager"

    property alias navigation: stackView

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: promptListScreen
    }

    Component {
        id: promptListScreen
        PromptListScreen {
            onEditPrompt: function(promptId) {
                stackView.push(promptEditScreen, {
                    "promptId": promptId,
                    "isEditing": true
                })
            }
            onCreateNewPrompt: {
                stackView.push(promptEditScreen, {
                    "promptId": -1,
                    "isEditing": false
                })
            }
            onFillPlaceholders: function(promptId) {
                stackView.push(placeholderFillingScreen, {
                    "promptId": promptId
                })
            }
            onFillPlaceholdersWithContent: function(promptId, content) {
                stackView.push(placeholderFillingScreen, {
                    "promptId": promptId,
                    "content": content
                })
            }
        }
    }

    Component {
        id: promptEditScreen
        PromptEditScreen {
            onSaved: {
                stackView.pop()
            }
            onCancelled: {
                stackView.pop()
            }
            onFillPlaceholders: function(content) {
                stackView.push(placeholderFillingScreen, {
                    "promptId": -1,
                    "content": content
                })
            }
        }
    }

    Component {
        id: placeholderFillingScreen
        PlaceholderFillingScreen {
            onFinished: {
                stackView.pop()
            }
            onBack: {
                stackView.pop()
            }
        }
    }
}