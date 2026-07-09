#include "ModelDownloader.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

ModelDownloader::ModelDownloader(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{
}

void ModelDownloader::downloadModel(const QString &url, const QString &savePath) {
    if (m_downloading) return;

    QDir().mkpath(QFileInfo(savePath).absolutePath());

    m_file = new QFile(savePath, this);
    if (!m_file->open(QIODevice::WriteOnly)) {
        emit downloadError("Cannot open file: " + savePath);
        return;
    }

    QUrl murl(url); QNetworkRequest req(murl);
    m_reply = m_nam->get(req);

    connect(m_reply, &QNetworkReply::downloadProgress, this, [this](qint64 recv, qint64 total) {
        m_received = recv;
        m_total = total;
        emit progressChanged();
    });

    connect(m_reply, &QNetworkReply::readyRead, this, [this]() {
        if (m_file) m_file->write(m_reply->readAll());
    });

    connect(m_reply, &QNetworkReply::finished, this, [this]() {
        m_reply->deleteLater();
        m_reply = nullptr;
        if (m_file) {
            m_file->close();
            m_file->deleteLater();
            m_file = nullptr;
        }
        m_downloading = false;
        emit downloadingChanged();
        emit downloadFinished(m_currentFile);
    });

    connect(m_reply, &QNetworkReply::errorOccurred,
            this, [this](QNetworkReply::NetworkError) {
        emit downloadError(m_reply->errorString());
    });

    m_currentFile = savePath;
    m_downloading = true;
    m_received = 0;
    m_total = 0;
    emit downloadingChanged();
    emit fileChanged();
}

void ModelDownloader::cancelDownload() {
    if (m_reply) {
        m_reply->abort();
    }
}

QString ModelDownloader::resolveSource(const QString &modelId, Source source) {
    switch (source) {
    case HuggingFace:
        return QString("https://huggingface.co/%1/resolve/main").arg(modelId);
    case HfMirror:
        return QString("https://hf-mirror.com/%1/resolve/main").arg(modelId);
    case ModelScope:
        return QString("https://modelscope.cn/%1/resolve/main").arg(modelId);
    }
    return {};
}
