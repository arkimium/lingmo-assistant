#include "OllamaProvider.h"
#include <QJsonDocument>
#include <QJsonObject>

OllamaProvider::OllamaProvider(const QString &providerId,
                                 const QString &name,
                                 const QString &desc,
                                 bool needsKey,
                                 bool local,
                                 QObject *parent)
    : AIProvider(parent)
    , m_id(providerId)
    , m_name(name)
    , m_desc(desc)
    , m_requiresKey(needsKey)
    , m_local(local)
    , m_network(this)
{
}

void OllamaProvider::setEndpoint(const QString &baseUrl, const QString &apiKey) {
    m_baseUrl = baseUrl;
    m_apiKey = apiKey;
}

bool OllamaProvider::isAvailable() const {
    return !m_baseUrl.isEmpty();
}

void OllamaProvider::fetchModels() {
    QMap<QString, QString> headers = buildHeaders();
    m_network.getJson(QUrl(m_baseUrl + "/api/tags"), headers,
        [this](bool success, const QByteArray &data, const QString &error) {
            if (!success) {
                emit modelsReady(QList<QString>());
                return;
            }
            QList<QString> models;
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonArray arr = doc.object()["models"].toArray();
            for (const auto &v : arr) {
                QString name = v.toObject()["name"].toString();
                if (!name.isEmpty()) models.append(name);
            }
            m_defaultModels = models;
            emit modelsReady(m_defaultModels);
        });
}

void OllamaProvider::sendMessage(const QString &conversationId,
                                  const QJsonArray &messages,
                                  const QJsonObject &params)
{
    Q_UNUSED(conversationId);

    m_parser = std::make_unique<StreamParser>(StreamParser::Format::JSON_Lines);
    m_parser->setCallbacks(
        [this](const QString &token) { emit tokenReceived(token); },
        [this](const QString &fullText, const QJsonObject &usage) {
            emit streamingFinished(fullText, usage);
        },
        [this](const QString &error) { emit errorOccurred(error); }
    );

    QString model = m_defaultModels.isEmpty() ? "llama3.2" : m_defaultModels.first();

    QJsonObject body;
    body["model"] = model;
    body["messages"] = messages;
    body["stream"] = true;

    QJsonObject options;
    if (params.contains("temperature")) {
        options["temperature"] = params["temperature"];
    }
    if (params.contains("max_tokens")) {
        options["num_predict"] = params["max_tokens"].toInt();
    }
    if (!options.isEmpty()) {
        body["options"] = options;
    }

    QUrl url(m_baseUrl + "/api/chat");
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

void OllamaProvider::cancelRequest() {
    if (m_activeReply) {
        m_activeReply->abort();
        m_activeReply = nullptr;
    }
}

QMap<QString, QString> OllamaProvider::buildHeaders() const {
    QMap<QString, QString> headers;
    if (!m_apiKey.isEmpty()) {
        headers["Authorization"] = "Bearer " + m_apiKey;
    }
    return headers;
}
