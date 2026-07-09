import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Item {
    id: root

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: Math.min(600, root.width)
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16
            anchors.margins: 24

            Label {
                text: qsTr("Settings")
                font.pixelSize: Theme.fontSizeHeadlineMedium
                font.bold: true
            }

            GroupBox {
                title: qsTr("Updates")
                Layout.fillWidth: true
                ColumnLayout {
                    RowLayout {
                        Label { text: qsTr("Check Interval"); Layout.preferredWidth: 140 }
                        ComboBox {
                            id: intervalCombo
                            model: [
                                { text: qsTr("Daily"), value: 1 },
                                { text: qsTr("Weekly"), value: 7 },
                                { text: qsTr("Monthly"), value: 30 }
                            ]
                            textRole: "text"
                            currentIndex: {
                                var v = Settings.updateCheckInterval || 1
                                if (v <= 1) return 0
                                if (v <= 7) return 1
                                return 2
                            }
                            onActivated: {
                                var val = model[currentIndex].value
                                UpdateManager.checkInterval = val
                            }
                        }
                    }
                    RowLayout {
                        Label { text: qsTr("Repo Owner"); Layout.preferredWidth: 140 }
                        TextField {
                            id: repoOwnerField
                            Layout.fillWidth: true
                            text: Settings.repoOwner
                            onEditingFinished: if (text) Settings.repoOwner = text
                        }
                    }
                    RowLayout {
                        Label { text: qsTr("Repo Name"); Layout.preferredWidth: 140 }
                        TextField {
                            id: repoNameField
                            Layout.fillWidth: true
                            text: Settings.repoName
                            onEditingFinished: if (text) Settings.repoName = text
                        }
                    }
                    RowLayout {
                        Label { text: qsTr("Last Checked"); Layout.preferredWidth: 140 }
                        Label {
                            text: UpdateManager.lastChecked
                            color: Theme.onSurfaceVariantColor
                        }
                        Item { Layout.fillWidth: true }
                        Button {
                            text: qsTr("Check Now")
                            flat: true
                            enabled: !UpdateManager.isChecking
                            onClicked: UpdateManager.checkForUpdates(true)
                        }
                    }
                }
            }

            GroupBox {
                title: qsTr("Appearance")
                Layout.fillWidth: true
                RowLayout {
                    Label { text: qsTr("Dark Mode"); Layout.fillWidth: true }
                    Switch {
                        checked: Settings.darkMode
                        onToggled: Settings.darkMode = checked
                    }
                }
            }

            GroupBox {
                title: qsTr("AI Provider")
                Layout.fillWidth: true
                ColumnLayout {
                    ComboBox {
                        id: providerCombo
                        Layout.fillWidth: true
                        model: ProviderManager.providerList || []
                        textRole: "name"
                        valueRole: "id"
                        currentIndex: {
                            var list = ProviderManager.providerList
                            if (!list) return 0
                            var id = Settings.activeProvider
                            for (var i = 0; i < list.length; i++) {
                                if (list[i].id === id) return i
                            }
                            return 0
                        }
                        onCurrentValueChanged: ProviderManager.setActiveProvider(currentValue)
                    }

                    ListView {
                        id: modelList
                        Layout.fillWidth: true
                        Layout.preferredHeight: Math.min(count * 36, 120)
                        model: ProviderManager.activeModels || []
                        delegate: ItemDelegate {
                            width: modelList.width
                            height: 36
                            text: modelData
                        }
                    }
                }
            }

            GroupBox {
                title: qsTr("API Keys")
                Layout.fillWidth: true
                visible: {
                    var p = providerCombo.currentValue
                    var list = ProviderManager.providerList
                    if (!list) return false
                    for (var i = 0; i < list.length; i++) {
                        if (list[i].id === p) return list[i].requiresApiKey
                    }
                    return false
                }
                ColumnLayout {
                    Label {
                        text: qsTr("DeepSeek API Key")
                        font.pixelSize: Theme.fontSizeLabelMedium
                    }
                    TextField {
                        id: deepseekKeyField
                        Layout.fillWidth: true
                        echoMode: TextInput.Password
                        text: Settings.deepseekApiKey
                        onEditingFinished: if (text) Settings.deepseekApiKey = text
                    }
                }
            }

            GroupBox {
                title: qsTr("Context")
                Layout.fillWidth: true
                ColumnLayout {
                    RowLayout {
                        Label { text: qsTr("History Token Limit"); Layout.fillWidth: true }
                        SpinBox {
                            from: 1024; to: 131072; stepSize: 1024
                            value: Settings.historyTokenLimit
                            onValueModified: Settings.historyTokenLimit = value
                        }
                    }
                    RowLayout {
                        Label { text: qsTr("Temperature"); Layout.fillWidth: true }
                        SpinBox {
                            from: 0; to: 200; stepSize: 10
                            value: Settings.temperature * 100
                            onValueModified: Settings.temperature = value / 100.0
                        }
                    }
                    RowLayout {
                        Label { text: qsTr("Max Output Tokens"); Layout.fillWidth: true }
                        SpinBox {
                            from: 256; to: 32768; stepSize: 256
                            value: Settings.maxOutputTokens
                            onValueModified: Settings.maxOutputTokens = value
                        }
                    }
                }
            }

            GroupBox {
                title: qsTr("Endpoints")
                Layout.fillWidth: true
                ColumnLayout {
                    Label { text: qsTr("DeepSeek"); font.pixelSize: Theme.fontSizeLabelMedium }
                    TextField {
                        id: deepseekField
                        Layout.fillWidth: true
                        text: Settings.deepseekEndpoint
                        onEditingFinished: if (text) Settings.deepseekEndpoint = text
                    }
                    Label { text: qsTr("Ollama Local"); font.pixelSize: Theme.fontSizeLabelMedium }
                    TextField {
                        id: ollamaField
                        Layout.fillWidth: true
                        text: Settings.ollamaEndpoint
                        onEditingFinished: if (text) Settings.ollamaEndpoint = text
                    }
                    Label { text: qsTr("Ollama Cloud"); font.pixelSize: Theme.fontSizeLabelMedium }
                    TextField {
                        id: ollamaCloudField
                        Layout.fillWidth: true
                        text: Settings.ollamaCloudEndpoint
                        onEditingFinished: if (text) Settings.ollamaCloudEndpoint = text
                    }
                }
            }
        }
    }
}
