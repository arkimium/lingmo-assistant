import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Popup {
    id: root
    modal: true
    closePolicy: Popup.NoAutoClose
    anchors.centerIn: parent
    width: Math.min(480, parent.width - 64)
    padding: 24

    background: Rectangle {
        radius: Theme.borderRadiusMedium
        color: Theme.surfaceColor
        border.color: Theme.outlineVariantColor
    }

    property string version: UpdateManager.latestVersion
    property string changelogText: UpdateManager.changelog
    property string downloadUrl: UpdateManager.downloadUrl

    onOpened: {
        version = UpdateManager.latestVersion
        changelogText = UpdateManager.changelog
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        Label {
            text: qsTr("Update Available!")
            font.pixelSize: 24
            font.bold: true
            color: Theme.primaryColor
        }

        Label {
            text: qsTr("LingmoOS AI Assistant v%1 (current: v%2)")
                .arg(root.version).arg(UpdateManager.currentVersion())
            font.pixelSize: 16
            color: Theme.onSurfaceColor
        }

        Label {
            text: qsTr("Changelog:")
            font.pixelSize: 14
            font.bold: true
            color: Theme.onSurfaceVariantColor
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            clip: true

            TextArea {
                id: changelogMarkdown
                readOnly: true
                text: root.changelogText || qsTr("(loading...)")
                textFormat: TextArea.MarkdownText
                wrapMode: TextArea.WordWrap
                font.pixelSize: 12
                background: Rectangle {
                    color: Theme.surfaceContainerLow
                    radius: Theme.borderRadiusMedium
                }
            }
        }

        ProgressBar {
            Layout.fillWidth: true
            visible: UpdateManager.downloadTotal > 0
            from: 0
            to: UpdateManager.downloadTotal
            value: UpdateManager.downloadReceived
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: 8

            Button {
                text: qsTr("Remind Later")
                flat: true
                onClicked: {
                    UpdateManager.remindLater()
                    root.close()
                }
            }

            Button {
                text: qsTr("Install Now")
                highlighted: true
                enabled: UpdateManager.downloadTotal <= 0
                onClicked: UpdateManager.downloadAndInstall()
            }
        }
    }

    Connections {
        target: UpdateManager
        function onInstallReady(debPath) { root.close() }
        function onErrorOccurred(error) { root.close() }
        function onUpdateAvailableChanged() {
            if (UpdateManager.updateAvailable) {
                root.version = UpdateManager.latestVersion
                root.changelogText = UpdateManager.changelog
                root.open()
            }
        }
    }
}
