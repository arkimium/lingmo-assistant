#pragma once
#include <QObject>
#include <QSettings>
#include <QString>
#include <QMap>

class Settings : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY darkModeChanged)
    Q_PROPERTY(QString activeProvider READ activeProvider WRITE setActiveProvider NOTIFY activeProviderChanged)
    Q_PROPERTY(QString activeModel READ activeModel WRITE setActiveModel NOTIFY activeModelChanged)
    Q_PROPERTY(int historyTokenLimit READ historyTokenLimit WRITE setHistoryTokenLimit NOTIFY historyTokenLimitChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(bool streamEnabled READ streamEnabled WRITE setStreamEnabled NOTIFY streamEnabledChanged)
    Q_PROPERTY(double temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(int maxOutputTokens READ maxOutputTokens WRITE setMaxOutputTokens NOTIFY maxOutputTokensChanged)
    Q_PROPERTY(QString deepseekEndpoint READ deepseekEndpoint WRITE setDeepseekEndpoint NOTIFY endpointsChanged)
    Q_PROPERTY(QString ollamaEndpoint READ ollamaEndpoint WRITE setOllamaEndpoint NOTIFY endpointsChanged)
    Q_PROPERTY(QString ollamaCloudEndpoint READ ollamaCloudEndpoint WRITE setOllamaCloudEndpoint NOTIFY endpointsChanged)
    Q_PROPERTY(bool configured READ isConfigured WRITE setConfigured NOTIFY configuredChanged)
    Q_PROPERTY(QString deepseekApiKey READ deepseekApiKey WRITE setDeepseekApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(int updateCheckInterval READ updateCheckInterval WRITE setUpdateCheckInterval NOTIFY updateCheckChanged)
    Q_PROPERTY(QString lastCheckTimestamp READ lastCheckTimestamp WRITE setLastCheckTimestamp NOTIFY updateCheckChanged)
    Q_PROPERTY(QString repoOwner READ repoOwner WRITE setRepoOwner NOTIFY updateCheckChanged)
    Q_PROPERTY(QString repoName READ repoName WRITE setRepoName NOTIFY updateCheckChanged)
    Q_PROPERTY(QString snoozeUntil READ snoozeUntil WRITE setSnoozeUntil NOTIFY updateCheckChanged)

public:
    explicit Settings(QObject *parent = nullptr);

    bool darkMode() const;
    void setDarkMode(bool v);

    QString activeProvider() const;
    void setActiveProvider(const QString &v);

    QString activeModel() const;
    void setActiveModel(const QString &v);

    int historyTokenLimit() const;
    void setHistoryTokenLimit(int v);

    QString language() const;
    void setLanguage(const QString &v);

    bool streamEnabled() const;
    void setStreamEnabled(bool v);

    double temperature() const;
    void setTemperature(double v);

    int maxOutputTokens() const;
    void setMaxOutputTokens(int v);

    QString deepseekEndpoint() const;
    void setDeepseekEndpoint(const QString &v);

    QString ollamaEndpoint() const;
    void setOllamaEndpoint(const QString &v);

    QString ollamaCloudEndpoint() const;
    void setOllamaCloudEndpoint(const QString &v);

    bool isConfigured() const;
    void setConfigured(bool v);

    QString deepseekApiKey() const;
    void setDeepseekApiKey(const QString &key);

    int updateCheckInterval() const;
    void setUpdateCheckInterval(int days);
    QString lastCheckTimestamp() const;
    void setLastCheckTimestamp(const QString &ts);
    QString repoOwner() const;
    void setRepoOwner(const QString &owner);
    QString repoName() const;
    void setRepoName(const QString &name);
    QString snoozeUntil() const;
    void setSnoozeUntil(const QString &dt);

    QString apiKey(const QString &providerId) const;
    void setApiKey(const QString &providerId, const QString &key);

    QString modelForProvider(const QString &providerId) const;
    void setModelForProvider(const QString &providerId, const QString &model);

    void saveConversations(const QJsonArray &convs);
    QJsonArray loadConversations() const;

signals:
    void darkModeChanged();
    void activeProviderChanged();
    void activeModelChanged();
    void historyTokenLimitChanged();
    void languageChanged();
    void streamEnabledChanged();
    void temperatureChanged();
    void maxOutputTokensChanged();
    void apiKeyChanged();
    void endpointsChanged();
    void configuredChanged();
    void updateCheckChanged();

private:
    QSettings m_settings;

    template<typename T>
    T read(const QString &key, const T &def) const;
    template<typename T>
    void write(const QString &key, const T &val);
};
