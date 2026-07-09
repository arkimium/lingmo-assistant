#pragma once
#include "AIProvider.h"
#include "../network/NetworkClient.h"
#include "../network/StreamParser.h"
#include <memory>

class OllamaProvider : public AIProvider {
    Q_OBJECT
public:
    explicit OllamaProvider(const QString &providerId,
                             const QString &name,
                             const QString &desc,
                             bool needsKey,
                             bool local,
                             QObject *parent = nullptr);

    QString id() const override { return m_id; }
    QString displayName() const override { return m_name; }
    QString description() const override { return m_desc; }
    bool requiresApiKey() const override { return m_requiresKey; }
    bool isLocal() const override { return m_local; }
    int contextLimit() const override { return 131072; }
    QList<QString> defaultModels() const override { return m_defaultModels; }
    bool isAvailable() const override;

    void setEndpoint(const QString &baseUrl, const QString &apiKey = QString());

    void fetchModels() override;
    void sendMessage(const QString &conversationId,
                     const QJsonArray &messages,
                     const QJsonObject &params) override;
    void cancelRequest() override;

private:
    QString m_id;
    QString m_name;
    QString m_desc;
    bool m_requiresKey;
    bool m_local;
    QString m_baseUrl;
    QString m_apiKey;
    QList<QString> m_defaultModels;
    NetworkClient m_network;
    std::unique_ptr<StreamParser> m_parser;
    QNetworkReply *m_activeReply = nullptr;

    QMap<QString, QString> buildHeaders() const;
};
