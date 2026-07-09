pragma Singleton
import QtQuick
import QtQuick.Controls.Material

QtObject {
    property bool darkMode: Settings.darkMode

    readonly property color primaryColor: M3Colors.md_theme_light_primary
    readonly property color onPrimaryColor: M3Colors.md_theme_light_onPrimary
    readonly property color primaryContainerColor: M3Colors.md_theme_light_primaryContainer
    readonly property color onPrimaryContainerColor: M3Colors.md_theme_light_onPrimaryContainer
    readonly property color secondaryColor: M3Colors.md_theme_light_secondary
    readonly property color onSecondaryColor: M3Colors.md_theme_light_onSecondary
    readonly property color secondaryContainerColor: M3Colors.md_theme_light_secondaryContainer
    readonly property color onSecondaryContainerColor: M3Colors.md_theme_light_onSecondaryContainer
    readonly property color tertiaryColor: M3Colors.md_theme_light_tertiary
    readonly property color onTertiaryColor: M3Colors.md_theme_light_onTertiary
    readonly property color errorColor: M3Colors.md_theme_light_error
    readonly property color onErrorColor: M3Colors.md_theme_light_onError
    readonly property color surfaceColor: M3Colors.md_theme_light_surface
    readonly property color onSurfaceColor: M3Colors.md_theme_light_onSurface
    readonly property color surfaceVariantColor: M3Colors.md_theme_light_surfaceVariant
    readonly property color onSurfaceVariantColor: M3Colors.md_theme_light_onSurfaceVariant
    readonly property color outlineColor: M3Colors.md_theme_light_outline
    readonly property color outlineVariantColor: M3Colors.md_theme_light_outlineVariant
    readonly property color surfaceContainerLow: M3Colors.md_theme_light_surfaceContainerLow
    readonly property color surfaceContainer: M3Colors.md_theme_light_surfaceContainer
    readonly property color surfaceContainerHigh: M3Colors.md_theme_light_surfaceContainerHigh
    readonly property color inverseSurface: M3Colors.md_theme_dark_surface
    readonly property color inverseOnSurface: M3Colors.md_theme_dark_onSurface
    readonly property color inversePrimary: M3Colors.md_theme_dark_primary

    readonly property string fontFamily: "Outfit"
    readonly property string fontFamilyMono: "JetBrains Mono"

    readonly property int fontSizeDisplayLarge: 57
    readonly property int fontSizeDisplayMedium: 45
    readonly property int fontSizeDisplaySmall: 36
    readonly property int fontSizeHeadlineLarge: 32
    readonly property int fontSizeHeadlineMedium: 28
    readonly property int fontSizeHeadlineSmall: 24
    readonly property int fontSizeTitleLarge: 22
    readonly property int fontSizeTitleMedium: 16
    readonly property int fontSizeTitleSmall: 14
    readonly property int fontSizeBodyLarge: 16
    readonly property int fontSizeBodyMedium: 14
    readonly property int fontSizeBodySmall: 12
    readonly property int fontSizeLabelLarge: 14
    readonly property int fontSizeLabelMedium: 12
    readonly property int fontSizeLabelSmall: 11

    readonly property real borderRadiusSmall: 8
    readonly property real borderRadiusMedium: 12
    readonly property real borderRadiusLarge: 16
    readonly property real borderRadiusXLarge: 24
    readonly property real borderRadiusFull: 999

    property string seedColor: "#6750A4"

    function alpha(color, opacity) {
        return Qt.rgba(color.r, color.g, color.b, opacity)
    }
}
