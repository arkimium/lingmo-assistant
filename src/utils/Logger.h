#pragma once
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class Logger : public QObject {
    Q_OBJECT
public:
    static Logger &instance();

    void init(const QString &logPath = QString());
    void log(const QString &category, const QString &message);
    void info(const QString &message);
    void warn(const QString &message);
    void error(const QString &message);

private:
    Logger(QObject *parent = nullptr);
    QFile m_file;
    QTextStream m_stream;
    QMutex m_mutex;
};
