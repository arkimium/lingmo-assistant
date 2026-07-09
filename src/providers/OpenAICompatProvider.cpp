#include "OpenAICompatProvider.h"
#include <QJsonDocument>
#include <QJsonObject>

OpenAICompatProvider::OpenAICompatProvider(const QString &providerId,
                                             const QString &name,
                                             const QString &desc,
                                             bool needsKey,
                                             bool local,
                                             int ctxLimit,
                                             QObject *parent)
    : AIProvider(parent)
    , m_id(providerId)
    , m_name(name)
    , m_desc(desc)
    , m_requiresKey(needsKey)
    , m_local(local)
    , m_ctxLimit(ctxLimit)
    , m_network(this)
{
}

void OpenAICompatProvider::setEndpoint(const QString &baseUrl, const QString &chatPath,
                                        const QString &apiKey) {
    m_baseUrl = baseUrl;
    m_chatPath = chatPath;
    m_apiKey = apiKey;
}

bool OpenAICompatProvider::isAvailable() const {
    if (m_requiresKey && m_apiKey.isEmpty()) return false;
    return !m_baseUrl.isEmpty();
}

void OpenAICompatProvider::fetchModels() {
    QMap<QString, QString> headers = buildHeaders();
    m_network.getJson(QUrl(m_baseUrl + "/models"), headers,
        [this](bool success, const QByteArray &data, const QString &error) {
            if (!success) {
                emit errorOccurred("Failed to fetch models: " + error);
                return;
            }
            QList<QString> models;
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonArray arr = doc.object()["data"].toArray();
            for (const auto &v : arr) {
                QString id = v.toObject()["id"].toString();
                if (!id.isEmpty()) models.append(id);
            }
            if (!models.isEmpty()) {
                m_defaultModels = models;
            }
            emit modelsReady(m_defaultModels);
        });
}

void OpenAICompatProvider::sendMessage(const QString &conversationId,
                                        const QJsonArray &messages,
                                        const QJsonObject &params)
{
    Q_UNUSED(conversationId);

    m_parser = std::make_unique<StreamParser>(StreamParser::Format::SSE_OpenAI);
    m_parser->setCallbacks(
        [this](const QString &token) { emit tokenReceived(token); },
        [this](const QString &fullText, const QJsonObject &usage) {
            emit streamingFinished(fullText, usage);
        },
        [this](const QString &error) { emit errorOccurred(error); }
    );

    QJsonObject body;
    body["model"] = m_defaultModels.isEmpty() ? "deepseek-chat" : m_defaultModels.first();
    body["messages"] = messages;
    body["stream"] = true;
    if (params.contains("temperature")) body["temperature"] = params["temperature"];
    if (params.contains("max_tokens")) body["max_tokens"] = params["max_tokens"];

    QUrl url(m_baseUrl + m_chatPath);
    QMap<QString, QString> headers = buildHeaders();

    emit streamingStarted();

    m_activeReply = m_network.postJsonStream(url, body, headers,
        [this](const QByteArray &chunk) {
            if (m_parser) m_parser->feed(chunk);
        },
        [this](bool success, const QString &error) {
            m_activeReply = nullptr;
            if (!success) {
                emit errorOccurred(error);
            }
        });
}

void OpenAICompatProvider::cancelRequest() {
    if (m_activeReply) {
        m_activeReply->abort();
        m_activeReply = nullptr;
    }
}

QMap<QString, QString> OpenAICompatProvider::buildHeaders() const {
    QMap<QString, QString> headers;
    if (!m_apiKey.isEmpty()) {
        headers["Authorization"] = "Bearer " + m_apiKey;
    }
    return headers;
}
