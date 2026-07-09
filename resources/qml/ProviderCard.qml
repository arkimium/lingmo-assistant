import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ItemDelegate {
    id: root
    property string providerName: ""
    property string providerDesc: ""
    property bool isActive: false

    width: parent ? parent.width : 280
    height: 72

    background: Rectangle {
        radius: Theme.borderRadiusMedium
        color: isActive ? Theme.primaryContainerColor : Theme.surfaceContainerLow
        border.color: isActive ? Theme.primaryColor : Theme.outlineVariantColor
    }

    contentItem: ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 2

        Label {
            text: providerName
            font.pixelSize: Theme.fontSizeBodyLarge
            font.bold: true
            color: isActive ? Theme.onPrimaryContainerColor : Theme.onSurfaceColor
        }
        Label {
            text: providerDesc
            font.pixelSize: Theme.fontSizeLabelSmall
            color: isActive ? Theme.onPrimaryContainerColor : Theme.onSurfaceVariantColor
            elide: Text.ElideRight
        }
    }
}
