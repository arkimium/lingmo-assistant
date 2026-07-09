pragma Singleton
import QtQuick
import QtQml

QtObject {
    id: root

    property string fontsDir: ""

    property bool allLoaded: outfitLoader.status === FontLoader.Ready
                           && notoLoader.status === FontLoader.Ready
                           && monoLoader.status === FontLoader.Ready

    readonly property bool outfitReady: outfitLoader.status === FontLoader.Ready
    readonly property bool notoReady: notoLoader.status === FontLoader.Ready
    readonly property bool monoReady: monoLoader.status === FontLoader.Ready

    function fontPath(name) {
        if (!fontsDir) return ""
        if (fontsDir.endsWith("/")) {
            return fontsDir + name + ".ttf"
        }
        return fontsDir + "/" + name + ".ttf"
    }

    FontLoader {
        id: outfitLoader
        source: fontsDir ? root.fontPath("Outfit[wght]") : ""
    }

    FontLoader {
        id: notoLoader
        source: fontsDir ? root.fontPath("NotoSansSC[wght]") : ""
    }

    FontLoader {
        id: monoLoader
        source: fontsDir ? root.fontPath("JetBrainsMono[wght]") : ""
    }

    function reload() {
        outfitLoader.source = ""
        notoLoader.source = ""
        monoLoader.source = ""
        Qt.callLater(function() {
            outfitLoader.source = root.fontPath("Outfit[wght]")
            notoLoader.source = root.fontPath("NotoSansSC[wght]")
            monoLoader.source = root.fontPath("JetBrainsMono[wght]")
        })
    }
}
