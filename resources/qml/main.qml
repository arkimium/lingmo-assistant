import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 1100
    height: 750
    minimumWidth: 800
    minimumHeight: 550
    visible: true
    title: qsTr("Lingmo AI Assistant")

    property string fontsDir: ""


    Material.theme: Settings.darkMode ? Material.Dark : Material.Light
    Material.primary: Theme.primaryColor
    Material.accent: Theme.accentColor


    color: Theme.surfaceColor

    SetupWizard {
        id: initScreen
        anchors.fill: parent
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
        visible: Settings.configured

        Sidebar {
            id: sidebar
            Layout.preferredWidth: 280
            Layout.fillHeight: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.surfaceColor

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: Theme.outlineVariantColor
                }

                StackLayout {
                    id: mainStack
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: sidebar.currentView

                    ChatView {
                        id: chatView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    SettingsDialog {
                        id: settingsDialog
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    ModelDownloadDialog {
                        id: modelDownloadDialog
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }

    UpdateDialog { id: updateDialog }
}
