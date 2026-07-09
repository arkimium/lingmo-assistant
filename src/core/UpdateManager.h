#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDateTime>
#include <QFile>

class Settings;

class UpdateManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(int checkInterval READ checkInterval WRITE setCheckInterval NOTIFY settingsChanged)
    Q_PROPERTY(QString lastChecked READ lastChecked NOTIFY lastCheckedChanged)
    Q_PROPERTY(bool isChecking READ isChecking NOTIFY isCheckingChanged)
    Q_PROPERTY(bool updateAvailable READ updateAvailable NOTIFY updateAvailableChanged)
    Q_PROPERTY(QString latestVersion READ latestVersion NOTIFY updateAvailableChanged)
    Q_PROPERTY(QString changelog READ changelog NOTIFY updateAvailableChanged)
    Q_PROPERTY(QString downloadUrl READ downloadUrl NOTIFY updateAvailableChanged)
    Q_PROPERTY(qint64 downloadReceived READ downloadReceived NOTIFY downloadProgressChanged)
    Q_PROPERTY(qint64 downloadTotal READ downloadTotal NOTIFY downloadProgressChanged)

public:
    explicit UpdateManager(Settings *settings, QObject *parent = nullptr);

    int checkInterval() const;
    void setCheckInterval(int days);
    QString lastChecked() const;
    bool isChecking() const { return m_checking; }
    bool updateAvailable() const { return m_hasUpdate; }
    QString latestVersion() const { return m_latestVersion; }
    QString changelog() const { return m_changelog; }
    QString downloadUrl() const { return m_downloadUrl; }
    qint64 downloadReceived() const { return m_dlReceived; }
    qint64 downloadTotal() const { return m_dlTotal; }

    Q_INVOKABLE void checkForUpdates(bool userTriggered = false);
    Q_INVOKABLE void downloadAndInstall();
    Q_INVOKABLE void remindLater();
    Q_INVOKABLE QString currentVersion() const;

signals:
    void settingsChanged();
    void lastCheckedChanged();
    void isCheckingChanged();
    void updateAvailableChanged();
    void downloadProgressChanged();
    void checkCompleted(bool hasUpdate);
    void installReady(const QString &debPath);
    void errorOccurred(const QString &error);

private slots:
    void onTimerTick();
    void onReleasesFetched();
    void onDownloadFinished();

private:
    void startTimer();
    void saveLastChecked();
    void fetchReleases();

    Settings *m_settings;
    QNetworkAccessManager *m_nam;
    QTimer *m_timer;
    bool m_checking = false;
    bool m_hasUpdate = false;
    QString m_latestVersion;
    QString m_changelog;
    QString m_downloadUrl;
    QNetworkReply *m_activeReply = nullptr;
    QNetworkReply *m_downloadReply = nullptr;
    QFile *m_downloadFile = nullptr;
    qint64 m_dlReceived = 0;
    qint64 m_dlTotal = 0;
};
