import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Rectangle {
    id: root
    color: Theme.surfaceColor
    z: 10
    anchors.fill: parent

    property bool shouldShow: !Settings.configured

    opacity: shouldShow ? (fadingOut ? 0.0 : 1.0) : 0.0
    visible: opacity > 0.01

    property bool fadingOut: false

    Behavior on opacity {
        NumberAnimation { duration: 400; easing.type: Easing.OutCubic }
    }

    Connections {
        target: SetupController
        function onIsCompleteChanged() {
            if (SetupController.isComplete && root.shouldShow) {
                fadeTimer.start()
            }
        }
    }

    Timer {
        id: fadeTimer
        interval: 800
        onTriggered: {
            root.fadingOut = true
            hideTimer.start()
        }
    }

    Timer {
        id: hideTimer
        interval: 500
        onTriggered: {
            root.shouldShow = false
        }
    }

    Component.onCompleted: {
        if (!Settings.configured) {
            SetupController.start()
        } else {
            root.shouldShow = false
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(500, parent.width - 64)
        spacing: 0

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Lingmo AI Assistant")
            font.pixelSize: Theme.fontSizeHeadlineMedium
            font.bold: true
            color: Theme.primaryColor
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 8
            text: SetupController.currentStepText || qsTr("Initializing...")
            font.pixelSize: Theme.fontSizeTitleMedium
            color: Theme.onSurfaceColor
            elide: Text.ElideRight
            Layout.maximumWidth: parent ? parent.width - 32 : 400
        }

        Item { Layout.preferredHeight: 24 }

        ProgressBar {
            Layout.fillWidth: true
            Layout.preferredHeight: 8
            from: 0.0
            to: 1.0
            value: SetupController.progress
            indeterminate: SetupController.progress < 0.02
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 4
            text: Math.round(SetupController.progress * 100) + "%"
            font.pixelSize: Theme.fontSizeLabelLarge
            color: Theme.primaryColor
        }

        Item { Layout.preferredHeight: 24 }

        ListView {
            id: stepList
            Layout.fillWidth: true
            Layout.preferredHeight: stepList.count * 44
            interactive: false
            model: SetupController.steps

            delegate: Item {
                width: stepList.width
                height: 44

                RowLayout {
                    anchors.fill: parent
                    spacing: 10

                    Label {
                        Layout.preferredWidth: 24
                        text: {
                            switch (modelData.status) {
                            case 0: return "\u25CB"
                            case 1: return "\u25D0"
                            case 2: return "\u2705"
                            case 3: return "\u26A0"
                            default: return "\u25CB"
                            }
                        }
                        font.pixelSize: 16
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0

                        Label {
                            text: modelData.name
                            font.pixelSize: Theme.fontSizeBodyMedium
                            color: modelData.status === 3
                                   ? Theme.errorColor
                                   : Theme.onSurfaceColor
                        }

                        Label {
                            visible: modelData.detail && modelData.detail.length > 0
                            text: modelData.detail
                            font.pixelSize: Theme.fontSizeLabelSmall
                            color: Theme.onSurfaceVariantColor
                            elide: Text.ElideRight
                            Layout.maximumWidth: stepList.width - 80
                        }
                    }

                    Button {
                        visible: modelData.status === 3
                        Layout.preferredWidth: 60
                        Layout.preferredHeight: 28
                        text: qsTr("Retry")
                        flat: true
                        font.pixelSize: Theme.fontSizeLabelSmall
                        onClicked: {
                            SetupController.retryStep(modelData.id)
                        }
                    }
                }
            }
        }

        Item { Layout.preferredHeight: 16 }

        Label {
            Layout.alignment: Qt.AlignHCenter
            visible: SetupController.isComplete
            text: qsTr("Starting application...")
            font.pixelSize: Theme.fontSizeBodyMedium
            color: Theme.primaryColor
        }
    }
}
