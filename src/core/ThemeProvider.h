#pragma once
#include <QObject>
#include <QColor>

class ThemeProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QColor primaryColor READ primary CONSTANT)
    Q_PROPERTY(QColor onPrimaryColor READ onPrimary CONSTANT)
    Q_PROPERTY(QColor primaryContainerColor READ primaryContainer CONSTANT)
    Q_PROPERTY(QColor onPrimaryContainerColor READ onPrimaryContainer CONSTANT)
    Q_PROPERTY(QColor secondaryColor READ secondary CONSTANT)
    Q_PROPERTY(QColor onSecondaryColor READ onSecondary CONSTANT)
    Q_PROPERTY(QColor secondaryContainerColor READ secondaryContainer CONSTANT)
    Q_PROPERTY(QColor onSecondaryContainerColor READ onSecondaryContainer CONSTANT)
    Q_PROPERTY(QColor surfaceColor READ surface CONSTANT)
    Q_PROPERTY(QColor onSurfaceColor READ onSurface CONSTANT)
    Q_PROPERTY(QColor surfaceVariantColor READ surfaceVariant CONSTANT)
    Q_PROPERTY(QColor onSurfaceVariantColor READ onSurfaceVariant CONSTANT)
    Q_PROPERTY(QColor surfaceContainerLow READ surfaceContainerLow CONSTANT)
    Q_PROPERTY(QColor surfaceContainerHigh READ surfaceContainerHigh CONSTANT)
    Q_PROPERTY(QColor outlineColor READ outline CONSTANT)
    Q_PROPERTY(QColor outlineVariantColor READ outlineVariant CONSTANT)
    Q_PROPERTY(QColor errorColor READ error CONSTANT)
    Q_PROPERTY(QColor accentColor READ accent CONSTANT)
    Q_PROPERTY(qreal borderRadiusMedium READ borderRadius CONSTANT)
    Q_PROPERTY(int fontSizeHeadlineMedium READ fontSizeHeadlineMedium CONSTANT)
    Q_PROPERTY(int fontSizeHeadlineSmall READ fontSizeHeadlineSmall CONSTANT)
    Q_PROPERTY(int fontSizeTitleLarge READ fontSizeTitleLarge CONSTANT)
    Q_PROPERTY(int fontSizeTitleMedium READ fontSizeTitleMedium CONSTANT)
    Q_PROPERTY(int fontSizeBodyLarge READ fontSizeBodyLarge CONSTANT)
    Q_PROPERTY(int fontSizeBodyMedium READ fontSizeBodyMedium CONSTANT)
    Q_PROPERTY(int fontSizeBodySmall READ fontSizeBodySmall CONSTANT)
    Q_PROPERTY(int fontSizeLabelLarge READ fontSizeLabelLarge CONSTANT)
    Q_PROPERTY(int fontSizeLabelMedium READ fontSizeLabelMedium CONSTANT)
    Q_PROPERTY(int fontSizeLabelSmall READ fontSizeLabelSmall CONSTANT)
public:
    explicit ThemeProvider(QObject *parent = nullptr);
    QColor primary() const { return QColor("#6750A4"); }
    QColor onPrimary() const { return QColor("#FFFFFF"); }
    QColor primaryContainer() const { return QColor("#EADDFF"); }
    QColor onPrimaryContainer() const { return QColor("#21005E"); }
    QColor secondary() const { return QColor("#625B71"); }
    QColor onSecondary() const { return QColor("#FFFFFF"); }
    QColor secondaryContainer() const { return QColor("#E8DEF8"); }
    QColor onSecondaryContainer() const { return QColor("#1E192B"); }
    QColor surface() const { return QColor("#FFFBFE"); }
    QColor onSurface() const { return QColor("#1C1B1F"); }
    QColor surfaceVariant() const { return QColor("#E7E0EC"); }
    QColor onSurfaceVariant() const { return QColor("#49454F"); }
    QColor surfaceContainerLow() const { return QColor("#F7F2FA"); }
    QColor surfaceContainerHigh() const { return QColor("#ECE6F0"); }
    QColor outline() const { return QColor("#79747E"); }
    QColor outlineVariant() const { return QColor("#CAC4D0"); }
    QColor error() const { return QColor("#B3261E"); }
    QColor accent() const { return QColor("#6750A4"); }
    qreal borderRadius() const { return 12.0; }
    int fontSizeHeadlineMedium() const { return 28; }
    int fontSizeHeadlineSmall() const { return 24; }
    int fontSizeTitleLarge() const { return 22; }
    int fontSizeTitleMedium() const { return 16; }
    int fontSizeBodyLarge() const { return 16; }
    int fontSizeBodyMedium() const { return 14; }
    int fontSizeBodySmall() const { return 12; }
    int fontSizeLabelLarge() const { return 14; }
    int fontSizeLabelMedium() const { return 12; }
    int fontSizeLabelSmall() const { return 11; }
};
