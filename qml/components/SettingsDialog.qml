import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform 1.1 as Platform

Dialog {
    id: root
    title: "Settings"
    modal: true
    standardButtons: Dialog.Ok

    property alias promptsPath: pathField.text

    ColumnLayout {
        spacing: 20
        anchors.fill: parent

        Label {
            text: "Prompts Location"
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            
            TextField {
                id: pathField
                Layout.fillWidth: true
                text: settingsManager.promptsPath
                readOnly: true
            }

            Button {
                text: "Browse..."
                onClicked: folderDialog.open()
            }
        }
        
        Label {
            text: "Note: Changing the path will reload prompts immediately."
            font.pixelSize: 12
            color: "#888"
        }
    }

    Platform.FolderDialog {
        id: folderDialog
        title: "Select Prompts Folder"
        currentFolder: "file://" + settingsManager.promptsPath
        onAccepted: {
            var path = currentFolder.toString();
            // Remove file:// prefix if present
            if (path.startsWith("file://")) {
                path = path.substring(7);
            }
            settingsManager.promptsPath = path;
        }
    }
}
