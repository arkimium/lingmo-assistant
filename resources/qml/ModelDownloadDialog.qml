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
                text: qsTr("Model Management")
                font.pixelSize: Theme.fontSizeHeadlineMedium
                font.bold: true
            }

            GroupBox {
                title: qsTr("Download Model")
                Layout.fillWidth: true
                ColumnLayout {
                    Label {
                        text: qsTr("Download models from HuggingFace or mirrors for llama.cpp")
                        font.pixelSize: Theme.fontSizeBodySmall
                        color: Theme.onSurfaceVariantColor
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }
                    RowLayout {
                        Label { text: qsTr("Source"); Layout.preferredWidth: 80 }
                        ComboBox {
                            id: sourceCombo
                            model: [
                                { text: "hf-mirror (China)", value: 1 },
                                { text: "HuggingFace", value: 0 },
                                { text: "ModelScope", value: 2 }
                            ]
                            textRole: "text"
                            currentIndex: 0
                        }
                    }
                    RowLayout {
                        Label { text: qsTr("Model ID"); Layout.preferredWidth: 80 }
                        TextField {
                            id: modelIdField
                            Layout.fillWidth: true
                            placeholderText: "Qwen/Qwen2.5-7B-Instruct-GGUF"
                        }
                    }
                    RowLayout {
                        Label { text: qsTr("File"); Layout.preferredWidth: 80 }
                        TextField {
                            id: modelFileField
                            Layout.fillWidth: true
                            placeholderText: "qwen2.5-7b-instruct-q4_k_m.gguf"
                        }
                    }
                    Button {
                        text: downloader.isDownloading ? qsTr("Downloading...") : qsTr("Download")
                        enabled: modelIdField.text.length > 0 && modelFileField.text.length > 0
                                 && !downloader.isDownloading
                        onClicked: {
                            var srcIndex = sourceCombo.model[sourceCombo.currentIndex].value
                            var srcUrl
                            if (srcIndex === 0) {
                                srcUrl = "https://huggingface.co/" + modelIdField.text + "/resolve/main"
                            } else if (srcIndex === 1) {
                                srcUrl = "https://hf-mirror.com/" + modelIdField.text + "/resolve/main"
                            } else {
                                srcUrl = "https://modelscope.cn/" + modelIdField.text + "/resolve/main"
                            }
                            var fullUrl = srcUrl + "/" + modelFileField.text
                        }
                    }
                    ProgressBar {
                        Layout.fillWidth: true
                        visible: downloader.isDownloading
                        from: 0
                        to: downloader.totalBytes > 0 ? downloader.totalBytes : 1
                        value: downloader.receivedBytes
                    }
                    Label {
                        visible: downloader.isDownloading
                        text: qsTr("%1 / %2 MB").arg(
                            (downloader.receivedBytes / 1048576).toFixed(1))
                            .arg((downloader.totalBytes / 1048576).toFixed(1))
                        font.pixelSize: Theme.fontSizeLabelSmall
                    }
                }
            }

            GroupBox {
                title: qsTr("Local Models")
                Layout.fillWidth: true
                ListView {
                    id: localModelList
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    model: ListModel { id: localModels }
                    delegate: ItemDelegate {
                        width: parent ? parent.width : 200
                        height: 48
                        text: modelData.name + " (" + parseFloat(modelData.fileSizeMB).toFixed(0) + " MB)"
                    }
                }
            }

            GroupBox {
                title: qsTr("llama.cpp Server")
                Layout.fillWidth: true
                ColumnLayout {
                    RowLayout {
                        Label { text: qsTr("Status"); Layout.preferredWidth: 120 }
                        Label {
                            text: llamaCppStatus
                            font.bold: true
                            color: llamaCppStatus === "running" ? "green" : Theme.onSurfaceColor
                        }
                    }
                    RowLayout {
                        Button {
                            text: llamaCppStatus === "running" ? qsTr("Stop Server") : qsTr("Start Server")
                            onClicked: {
                                if (llamaCppStatus === "running") {
                                } else {
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    property string llamaCppStatus: "stopped"

    QtObject {
        id: downloader
        property bool isDownloading: false
        property int receivedBytes: 0
        property int totalBytes: 0
    }
}
