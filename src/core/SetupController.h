#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class Settings;

struct SetupStep {
    enum Status { Pending, Running, Done, Failed };
    QString id;
    QString name;
    Status status = Pending;
    QString detail;
};

class SetupController : public QObject {
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString currentStepText READ currentStepText NOTIFY currentStepTextChanged)
    Q_PROPERTY(bool isComplete READ isComplete NOTIFY isCompleteChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(QVariantList steps READ stepsVariant NOTIFY stepsChanged)

public:
    explicit SetupController(Settings *settings, QObject *parent = nullptr);

    qreal progress() const { return m_progress; }
    QString currentStepText() const { return m_currentText; }
    bool isComplete() const { return m_complete; }
    bool isRunning() const { return m_running; }
    QVariantList stepsVariant() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void retryStep(const QString &stepId);

signals:
    void progressChanged();
    void currentStepTextChanged();
    void isCompleteChanged();
    void isRunningChanged();
    void stepsChanged();

private slots:
    void advanceStep();

private:
    void runDependencies();
    void runFonts();
    void runOllama();
    void runLlamaCpp();
    void runFinalize();

    void downloadNextFont();
    void onFontDownloaded();
    void onFontDownloadProgress(qint64 received, qint64 total);

    void setStepStatus(const QString &id, SetupStep::Status status, const QString &detail = QString());
    void updateProgress();

    Settings *m_settings;
    QList<SetupStep> m_steps;
    int m_currentStepIdx = -1;
    bool m_complete = false;
    bool m_running = false;
    qreal m_progress = 0.0;
    QString m_currentText;

    QNetworkAccessManager *m_nam;
    QNetworkReply *m_fontReply = nullptr;
    int m_fontDownloadIdx = 0;
    qreal m_fontStepProgress = 0.0;
    QStringList m_fontUrls;
    QString m_fontsDir;
    static const int TOTAL_STEPS = 5;
};
