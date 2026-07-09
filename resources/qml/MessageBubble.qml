import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property string role: "user"
    property string content: ""
    implicitHeight: bubble.height + 8

    Rectangle {
        id: bubble
        anchors {
            left: role === "assistant" ? parent.left : undefined
            right: role === "user" ? parent.right : undefined
            margins: 8
        }
        width: Math.min(implicitWidth, parent.width * 0.8)
        radius: Theme.borderRadiusMedium
        color: role === "user"
               ? Theme.primaryContainerColor
               : Theme.surfaceContainerHigh

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 4

            Label {
                text: role === "user" ? qsTr("You") : qsTr("AI")
                font.pixelSize: Theme.fontSizeLabelSmall
                font.bold: true
                color: role === "user"
                       ? Theme.onPrimaryContainerColor
                       : Theme.primaryColor
            }

            Text {
                Layout.fillWidth: true
                text: content
                textFormat: Text.MarkdownText
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeBodyMedium
                color: role === "user"
                       ? Theme.onPrimaryContainerColor
                       : Theme.onSurfaceColor
            }
        }
    }
}
