#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFont>

#include "core/ChatController.h"
#include "core/UpdateManager.h"
#include "core/Settings.h"
#include "core/SetupController.h"
#include "core/ThemeProvider.h"
#include "providers/ProviderManager.h"
#include "utils/Logger.h"
#include "utils/Platform.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("LingmoOS AI Assistant");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("LingmoOS");

    Logger::instance().init();

    QQuickStyle::setStyle("Material");

    Settings settings;
    ProviderManager providerManager;
    providerManager.init(&settings);

    ChatController chatController(&settings, &providerManager);
    SetupController setupController(&settings);
    UpdateManager updateManager(&settings);
    ThemeProvider themeProvider;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("ChatController", &chatController);
    engine.rootContext()->setContextProperty("Settings", &settings);
    engine.rootContext()->setContextProperty("ProviderManager", &providerManager);
    engine.rootContext()->setContextProperty("UpdateManager", &updateManager);
    engine.rootContext()->setContextProperty("SetupController", &setupController);
    engine.rootContext()->setContextProperty("Platform", new QObject(&app));
    engine.rootContext()->setContextProperty("Theme", &themeProvider);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    QObject *rootObj = engine.rootObjects().isEmpty() ? nullptr : engine.rootObjects().first();
    if (rootObj) {
        rootObj->setProperty("fontsDir", Platform::fontsDir());
    }

    return app.exec();
}
