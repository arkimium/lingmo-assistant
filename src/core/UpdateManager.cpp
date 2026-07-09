#include "UpdateManager.h"
#include "Settings.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>

UpdateManager::UpdateManager(Settings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
    , m_nam(new QNetworkAccessManager(this))
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &UpdateManager::onTimerTick);
    startTimer();
}

int UpdateManager::checkInterval() const {
    return m_settings->updateCheckInterval();
}

void UpdateManager::setCheckInterval(int days) {
    m_settings->setUpdateCheckInterval(days);
    startTimer();
    emit settingsChanged();
}

QString UpdateManager::lastChecked() const {
    QString ts = m_settings->lastCheckTimestamp();
    if (ts.isEmpty()) return QStringLiteral("Never");
    QDateTime dt = QDateTime::fromString(ts, Qt::ISODate);
    return dt.isValid() ? dt.toString("yyyy-MM-dd hh:mm") : QStringLiteral("Never");
}

QString UpdateManager::currentVersion() const {
    return "0.1.0";
}

void UpdateManager::checkForUpdates(bool userTriggered) {
    if (m_checking) return;

    m_checking = true;
    emit isCheckingChanged();

    QDateTime now = QDateTime::currentDateTime();
    QDateTime last = QDateTime::fromString(m_settings->lastCheckTimestamp(), Qt::ISODate);

    if (!userTriggered && last.isValid()) {
        int hours = m_settings->updateCheckInterval() * 24;
        if (last.secsTo(now) < hours * 3600) {
            m_checking = false;
            emit isCheckingChanged();
            return;
        }
    }

    saveLastChecked();
    fetchReleases();
}

void UpdateManager::downloadAndInstall() {
    if (m_downloadUrl.isEmpty()) {
        emit errorOccurred("No download URL available");
        return;
    }

    QString tmpDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString debPath = tmpDir + "/lingmo-assistant-update.deb";

    m_downloadFile = new QFile(debPath, this);
    if (!m_downloadFile->open(QIODevice::WriteOnly)) {
        emit errorOccurred("Cannot write to " + debPath);
        return;
    }

    QUrl dlurl(m_downloadUrl); QNetworkRequest req(dlurl);
    m_downloadReply = m_nam->get(req);

    connect(m_downloadReply, &QNetworkReply::downloadProgress, this,
        [this](qint64 recv, qint64 total) {
            m_dlReceived = recv;
            m_dlTotal = total;
            emit downloadProgressChanged();
        });

    connect(m_downloadReply, &QNetworkReply::readyRead, this,
        [this]() {
            if (m_downloadFile) m_downloadFile->write(m_downloadReply->readAll());
        });

    connect(m_downloadReply, &QNetworkReply::finished, this, &UpdateManager::onDownloadFinished);
}

void UpdateManager::remindLater() {
    QDateTime now = QDateTime::currentDateTime();
    m_settings->setSnoozeUntil(now.addDays(1).toString(Qt::ISODate));
    m_hasUpdate = false;
    m_latestVersion.clear();
    m_changelog.clear();
    m_downloadUrl.clear();
    emit updateAvailableChanged();
}

void UpdateManager::onTimerTick() {
    checkForUpdates(false);
}

void UpdateManager::startTimer() {
    m_timer->stop();
    int intervalMs = m_settings->updateCheckInterval() * 24 * 3600 * 1000;
    m_timer->setInterval(intervalMs > 0 ? intervalMs : 3600000);
    m_timer->start();
}

void UpdateManager::saveLastChecked() {
    m_settings->setLastCheckTimestamp(
        QDateTime::currentDateTime().toString(Qt::ISODate));
    emit lastCheckedChanged();
}

void UpdateManager::fetchReleases() {
    QString owner = m_settings->repoOwner();
    QString name = m_settings->repoName();
    QString apiUrl = QString("https://api.github.com/repos/%1/%2/releases/latest")
                         .arg(owner, name);

    QUrl furl(apiUrl); QNetworkRequest req(furl);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Accept", "application/vnd.github+json");
    req.setRawHeader("User-Agent", "Lingmo-AI-Assistant-UpdateCheck/1.0");
    req.setTransferTimeout(10000);

    m_activeReply = m_nam->get(req);
    connect(m_activeReply, &QNetworkReply::finished, this, &UpdateManager::onReleasesFetched);
}

void UpdateManager::onReleasesFetched() {
    bool ok = m_activeReply->error() == QNetworkReply::NoError;
    QByteArray data = m_activeReply->readAll();
    m_activeReply->deleteLater();
    m_activeReply = nullptr;
    m_checking = false;

    if (!ok) {
        emit errorOccurred(QStringLiteral("GitHub API error"));
        emit isCheckingChanged();
        emit checkCompleted(false);
        return;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        emit errorOccurred("Failed to parse release data");
        emit isCheckingChanged();
        emit checkCompleted(false);
        return;
    }

    QJsonObject release = doc.object();
    QString tagName = release["tag_name"].toString().trimmed();
    if (tagName.isEmpty()) {
        emit isCheckingChanged();
        emit checkCompleted(false);
        return;
    }

    QString remoteVersion = tagName;
    if (remoteVersion.startsWith('v'))
        remoteVersion = remoteVersion.mid(1);

    QString localVersion = currentVersion();

    if (remoteVersion == localVersion) {
        m_hasUpdate = false;
        emit isCheckingChanged();
        emit checkCompleted(false);
        return;
    }

    m_hasUpdate = true;
    m_latestVersion = remoteVersion;
    m_changelog = release["body"].toString();

    QJsonArray assets = release["assets"].toArray();
    for (const auto &a : assets) {
        QJsonObject asset = a.toObject();
        QString name = asset["name"].toString();
        if (name.endsWith(".deb") && name.contains("amd64")) {
            m_downloadUrl = asset["browser_download_url"].toString();
            break;
        }
    }

    if (m_downloadUrl.isEmpty() && !assets.isEmpty()) {
        m_downloadUrl = assets.first().toObject()["browser_download_url"].toString();
    }

    emit isCheckingChanged();
    emit checkCompleted(true);
    emit updateAvailableChanged();
}

void UpdateManager::onDownloadFinished() {
    bool ok = m_downloadReply->error() == QNetworkReply::NoError;
    m_downloadReply->deleteLater();
    m_downloadReply = nullptr;

    if (m_downloadFile) {
        m_downloadFile->close();
        QString debPath = m_downloadFile->fileName();
        m_downloadFile->deleteLater();
        m_downloadFile = nullptr;

        if (ok) {
            emit installReady(debPath);
        } else {
            emit errorOccurred("Download failed");
        }
    }
}
