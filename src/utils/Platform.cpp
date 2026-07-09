#include "Platform.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSysInfo>

QString Platform::dataDir() {
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

QString Platform::configDir() {
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

QString Platform::cacheDir() {
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
}

QString Platform::fontsDir() {
    QString d = dataDir() + "/fonts";
    QDir().mkpath(d);
    return d;
}

QString Platform::modelsDir() {
    QString d = dataDir() + "/models";
    QDir().mkpath(d);
    return d;
}

bool Platform::isDebian() {
    return QFile::exists("/etc/debian_version");
}
