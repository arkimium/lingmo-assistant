import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Item {
    id: root
    property int currentView: 0

    Rectangle {
        anchors.fill: parent
        color: Theme.surfaceColor
        border.color: Theme.outlineVariantColor
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            color: Theme.surfaceContainerLow

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Label {
                    text: qsTr("Lingmo AI")
                    font.pixelSize: Theme.fontSizeTitleLarge
                    font.bold: true
                    color: Theme.primaryColor
                }

                Item { Layout.fillWidth: true }

                RoundButton {
                    Layout.preferredWidth: 36
                    Layout.preferredHeight: 36
                    text: "+"
                    font.pixelSize: Theme.fontSizeTitleMedium
                    onClicked: {
                        ChatController.createConversation()
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.outlineVariantColor
        }

        ListView {
            id: conversationList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            model: ChatController.conversations
            delegate: ItemDelegate {
                width: conversationList.width
                height: 56

                contentItem: ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 2
                    Label {
                        text: modelData.title || qsTr("New Chat")
                        font.pixelSize: Theme.fontSizeBodyMedium
                        elide: Text.ElideRight
                        color: modelData.id === ChatController.activeConversationId
                               ? Theme.primaryColor : Theme.onSurfaceColor
                    }
                    Label {
                        text: modelData.providerId + " - " + modelData.messageCount + " msgs"
                        font.pixelSize: Theme.fontSizeLabelSmall
                        color: Theme.onSurfaceVariantColor
                    }
                }

                background: Rectangle {
                    color: modelData.id === ChatController.activeConversationId
                           ? Theme.secondaryContainerColor : "transparent"
                }

                onClicked: {
                    ChatController.switchConversation(modelData.id)
                }

                MouseArea {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 32; height: 32
                    onClicked: {
                        ChatController.deleteConversation(modelData.id)
                    }
                    Label {
                        anchors.centerIn: parent
                        text: "\u00D7"
                        color: Theme.onSurfaceVariantColor
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.outlineVariantColor
        }

        ButtonGroup { id: navGroup }

        Repeater {
            model: [
                { label: qsTr("Chat"), icon: "\u2709", view: 0 },
                { label: qsTr("Settings"), icon: "\u2699", view: 1 },
                { label: qsTr("Models"), icon: "\u2B07", view: 2 }
            ]

            delegate: ItemDelegate {
                Layout.fillWidth: true
                Layout.preferredHeight: 44
                checked: root.currentView === modelData.view
                checkable: true
                ButtonGroup.group: navGroup

                contentItem: RowLayout {
                    spacing: 12
                    Label {
                        text: modelData.icon
                        font.pixelSize: Theme.fontSizeTitleMedium
                    }
                    Label {
                        text: modelData.label
                        font.pixelSize: Theme.fontSizeBodyMedium
                    }
                }

                onClicked: { root.currentView = modelData.view }
            }
        }
    }
}
