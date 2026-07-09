#pragma once
#include <QObject>
#include <QList>
#include <memory>

class AIProvider;
class Settings;

class ProviderManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList providerIds READ providerIds NOTIFY providersChanged)
    Q_PROPERTY(QStringList activeModels READ activeModels NOTIFY activeProviderChanged)

public:
    explicit ProviderManager(QObject *parent = nullptr);

    void init(Settings *settings);
    QStringList providerIds() const;

    AIProvider *provider(const QString &id) const;
    Q_INVOKABLE QVariantList providerList() const;
    Q_INVOKABLE void setActiveProvider(const QString &id);
    Q_INVOKABLE QString activeProviderId() const;

    QStringList activeModels() const;

signals:
    void providersChanged();
    void activeProviderChanged();

private:
    Settings *m_settings = nullptr;
    QList<AIProvider *> m_providers;
    QString m_activeProvider;
};
