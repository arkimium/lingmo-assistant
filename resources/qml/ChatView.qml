import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: Theme.surfaceColor

            Label {
                anchors.centerIn: parent
                text: ChatController.activeConversationId ? qsTr("Conversation") : qsTr("New Conversation")
                font.pixelSize: Theme.fontSizeTitleMedium
                color: Theme.onSurfaceColor
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.outlineVariantColor
        }

        ListView {
            id: messageList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 8
            boundsBehavior: Flickable.StopAtBounds
            verticalLayoutDirection: ListView.BottomToTop

            model: ListModel { id: chatModel }

            delegate: MessageBubble {
                width: messageList.width - 32
                anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                role: model.role
                content: model.content
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.outlineVariantColor
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: inputColumn.height + 24
            color: Theme.surfaceContainerLow

            ColumnLayout {
                id: inputColumn
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 12
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 44

                        TextArea {
                            id: messageInput
                            placeholderText: qsTr("Type a message...")
                            wrapMode: TextArea.Wrap
                            font.pixelSize: Theme.fontSizeBodyLarge
                            background: Rectangle {
                                radius: Theme.borderRadiusMedium
                                color: Theme.surfaceContainerHigh
                                border.color: Theme.outlineVariantColor
                            }
                            Keys.onPressed: function(event) {
                                if (event.key === Qt.Key_Return && !(event.modifiers & Qt.ShiftModifier)) {
                                    event.accepted = true
                                    sendMessage()
                                }
                            }
                        }
                    }

                    RoundButton {
                        id: sendButton
                        Layout.preferredWidth: 44
                        Layout.preferredHeight: 44
                        enabled: messageInput.text.trim().length > 0
                        text: "\u27A4"
                        font.pixelSize: 20
                        onClicked: sendMessage()
                    }
                }

                Label {
                    id: contextLabel
                    visible: ChatController.contextManager.utilizationPercent > 70
                    text: visible ? qsTr("Context: %1% used").arg(
                                       Math.round(ChatController.contextManager.utilizationPercent)) : ""
                    font.pixelSize: Theme.fontSizeLabelSmall
                    color: ChatController.contextManager.isWarning
                           ? Theme.errorColor : Theme.onSurfaceVariantColor
                    Layout.topMargin: 4
                }
            }
        }
    }

    function sendMessage() {
        var text = messageInput.text.trim()
        if (text.length === 0) return

        chatModel.insert(0, { role: "user", content: text })
        messageInput.text = ""
        ChatController.sendMessage(text)
    }

    function loadConversationMessages() {
        chatModel.clear()
        var conv = ChatController.getConversation(
            ChatController.activeConversationId)
        if (conv && conv.messages) {
            var msgs = conv.messages
            for (var i = msgs.length - 1; i >= 0; i--) {
                var m = msgs[i]
                chatModel.append({ role: m.role, content: m.content })
            }
        }
    }

    Connections {
        target: ChatController

        function onTokenReceived(token) {
            if (chatModel.count > 0 && chatModel.get(0).role === "assistant") {
                chatModel.setProperty(0, "content",
                    chatModel.get(0).content + token)
            } else {
                chatModel.insert(0, { role: "assistant", content: token })
            }
            messageList.positionViewAtBeginning()
        }

        function onResponseFinished(fullText) {
            if (chatModel.count > 0 && chatModel.get(0).role === "assistant") {
                chatModel.setProperty(0, "content", fullText)
            }
        }

        function onErrorOccurred(error) {
            chatModel.insert(0, { role: "assistant", content: qsTr("Error: ") + error })
        }

        function onActiveConversationChanged() {
            loadConversationMessages()
        }
    }

    Component.onCompleted: {
        loadConversationMessages()
    }
}
