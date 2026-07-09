#include "Settings.h"
#include <QJsonDocument>
#include <QJsonArray>

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_settings("LingmoOS", "Assistant")
{
}

bool Settings::darkMode() const           { return read("theme/dark", true); }
void Settings::setDarkMode(bool v)         { write("theme/dark", v); emit darkModeChanged(); }

QString Settings::activeProvider() const   { return read("general/provider", QStringLiteral("deepseek")); }
void Settings::setActiveProvider(const QString &v) { write("general/provider", v); emit activeProviderChanged(); }

QString Settings::activeModel() const      { return read("general/model", QStringLiteral("deepseek-chat")); }
void Settings::setActiveModel(const QString &v)    { write("general/model", v); emit activeModelChanged(); }

int Settings::historyTokenLimit() const    { return read("context/historyTokenLimit", 8192); }
void Settings::setHistoryTokenLimit(int v)  { write("context/historyTokenLimit", v); emit historyTokenLimitChanged(); }

QString Settings::language() const         { return read("general/language", QStringLiteral("en")); }
void Settings::setLanguage(const QString &v) { write("general/language", v); emit languageChanged(); }

bool Settings::streamEnabled() const       { return read("general/stream", true); }
void Settings::setStreamEnabled(bool v)     { write("general/stream", v); emit streamEnabledChanged(); }

double Settings::temperature() const       { return read("model/temperature", 0.7); }
void Settings::setTemperature(double v)     { write("model/temperature", v); emit temperatureChanged(); }

int Settings::maxOutputTokens() const      { return read("model/maxTokens", 4096); }
void Settings::setMaxOutputTokens(int v)    { write("model/maxTokens", v); emit maxOutputTokensChanged(); }

QString Settings::deepseekEndpoint() const { return read("endpoints/deepseek", QStringLiteral("https://api.deepseek.com")); }
void Settings::setDeepseekEndpoint(const QString &v) { write("endpoints/deepseek", v); emit endpointsChanged(); }

QString Settings::ollamaEndpoint() const   { return read("endpoints/ollama", QStringLiteral("http://localhost:11434")); }
void Settings::setOllamaEndpoint(const QString &v) { write("endpoints/ollama", v); emit endpointsChanged(); }

QString Settings::ollamaCloudEndpoint() const { return read("endpoints/ollamaCloud", QStringLiteral("https://ollama.ai")); }
void Settings::setOllamaCloudEndpoint(const QString &v) { write("endpoints/ollamaCloud", v); emit endpointsChanged(); }

QString Settings::deepseekApiKey() const { return apiKey(QStringLiteral("deepseek")); }

bool Settings::isConfigured() const { return read("system/configured", false); }
void Settings::setConfigured(bool v) { write("system/configured", v); emit configuredChanged(); }


int Settings::updateCheckInterval() const { return read("updates/checkInterval", 1); }
void Settings::setUpdateCheckInterval(int days) { write("updates/checkInterval", days); emit updateCheckChanged(); }
QString Settings::lastCheckTimestamp() const { return read("updates/lastCheck", QString()); }
void Settings::setLastCheckTimestamp(const QString &ts) { write("updates/lastCheck", ts); emit updateCheckChanged(); }
QString Settings::repoOwner() const { return read("updates/repoOwner", QStringLiteral("lingmo-os")); }
void Settings::setRepoOwner(const QString &v) { write("updates/repoOwner", v); emit updateCheckChanged(); }
QString Settings::repoName() const { return read("updates/repoName", QStringLiteral("lingmo-ai-assistant")); }
void Settings::setRepoName(const QString &v) { write("updates/repoName", v); emit updateCheckChanged(); }
QString Settings::snoozeUntil() const { return read("updates/snoozeUntil", QString()); }
void Settings::setSnoozeUntil(const QString &dt) { write("updates/snoozeUntil", dt); emit updateCheckChanged(); }

void Settings::setDeepseekApiKey(const QString &key) { setApiKey("deepseek", key); }

QString Settings::apiKey(const QString &providerId) const {
    return read(QString("keys/%1").arg(providerId), QString());
}

void Settings::setApiKey(const QString &providerId, const QString &key) {
    write(QString("keys/%1").arg(providerId), key);
    emit apiKeyChanged();
}

QString Settings::modelForProvider(const QString &providerId) const {
    return read(QString("models/%1").arg(providerId), QString());
}

void Settings::setModelForProvider(const QString &providerId, const QString &model) {
    write(QString("models/%1").arg(providerId), model);
}

void Settings::saveConversations(const QJsonArray &convs) {
    m_settings.setValue("conversations/list", QJsonDocument(convs).toJson(QJsonDocument::Compact));
}

QJsonArray Settings::loadConversations() const {
    QString data = m_settings.value("conversations/list").toString();
    if (data.isEmpty()) return {};
    return QJsonDocument::fromJson(data.toUtf8()).array();
}

template<typename T>
T Settings::read(const QString &key, const T &def) const {
    return m_settings.value(key, def).template value<T>();
}

template<typename T>
void Settings::write(const QString &key, const T &val) {
    m_settings.setValue(key, val);
}
