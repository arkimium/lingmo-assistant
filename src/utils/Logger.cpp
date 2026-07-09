#include "Logger.h"
#include <QDateTime>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>

Logger &Logger::instance() {
    static Logger inst;
    return inst;
}

Logger::Logger(QObject *parent) : QObject(parent) {}

void Logger::init(const QString &logPath) {
    QString path = logPath;
    if (path.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/lingmo.log";
    }
    QDir().mkpath(QFileInfo(path).absolutePath());
    m_file.setFileName(path);
    m_file.open(QIODevice::Append | QIODevice::Text);
    m_stream.setDevice(&m_file);
}

void Logger::log(const QString &category, const QString &message) {
    QMutexLocker locker(&m_mutex);
    QString line = QString("[%1] [%2] %3\n")
        .arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs))
        .arg(category)
        .arg(message);
    m_stream << line;
    m_stream.flush();
}

void Logger::info(const QString &message)  { log("INFO", message); }
void Logger::warn(const QString &message)  { log("WARN", message); }
void Logger::error(const QString &message) { log("ERROR", message); }
