#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class ModelDownloader : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isDownloading READ isDownloading NOTIFY downloadingChanged)
    Q_PROPERTY(qint64 receivedBytes READ receivedBytes NOTIFY progressChanged)
    Q_PROPERTY(qint64 totalBytes READ totalBytes NOTIFY progressChanged)
    Q_PROPERTY(QString currentFile READ currentFile NOTIFY fileChanged)

public:
    enum Source { HuggingFace, HfMirror, ModelScope };

    explicit ModelDownloader(QObject *parent = nullptr);

    bool isDownloading() const { return m_downloading; }
    qint64 receivedBytes() const { return m_received; }
    qint64 totalBytes() const { return m_total; }
    QString currentFile() const { return m_currentFile; }

    Q_INVOKABLE void downloadModel(const QString &url, const QString &savePath);
    Q_INVOKABLE void cancelDownload();
    Q_INVOKABLE static QString resolveSource(const QString &modelId, Source source);

signals:
    void downloadingChanged();
    void progressChanged();
    void fileChanged();
    void downloadFinished(const QString &filePath);
    void downloadError(const QString &error);

private:
    QNetworkAccessManager *m_nam;
    QNetworkReply *m_reply = nullptr;
    QFile *m_file = nullptr;
    bool m_downloading = false;
    qint64 m_received = 0;
    qint64 m_total = 0;
    QString m_currentFile;
};
