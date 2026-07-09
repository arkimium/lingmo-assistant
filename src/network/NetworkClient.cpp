#include "NetworkClient.h"
#include <QNetworkRequest>
#include <QJsonDocument>

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{
}

NetworkClient::~NetworkClient()
{
    cancelAll();
}

QNetworkReply *NetworkClient::postJson(const QUrl &url, const QJsonObject &body,
                                        const QMap<QString, QString> &headers,
                                        Callback callback)
{
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        req.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    QByteArray data = QJsonDocument(body).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = m_nam->post(req, data);
    m_activeReplies.append(reply);

    connect(reply, &QNetworkReply::finished, this, [reply, callback, this]() {
        reply->deleteLater();
        m_activeReplies.removeAll(reply);
        if (reply->error() != QNetworkReply::NoError) {
            if (callback)
                callback(false, QByteArray(), reply->errorString());
        } else {
            if (callback)
                callback(true, reply->readAll(), QString());
        }
    });

    return reply;
}

QNetworkReply *NetworkClient::postJsonStream(const QUrl &url, const QJsonObject &body,
                                               const QMap<QString, QString> &headers,
                                               StreamCallback onChunk,
                                               FinishCallback onFinish)
{
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Accept", "text/event-stream");
    req.setRawHeader("Cache-Control", "no-cache");
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        req.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    QByteArray data = QJsonDocument(body).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = m_nam->post(req, data);
    m_activeReplies.append(reply);

    connect(reply, &QNetworkReply::readyRead, this, [reply, onChunk]() {
        if (onChunk) {
            onChunk(reply->readAll());
        }
    });

    connect(reply, &QNetworkReply::finished, this, [reply, onFinish, this]() {
        reply->deleteLater();
        m_activeReplies.removeAll(reply);
        if (reply->error() != QNetworkReply::NoError) {
            if (onFinish)
                onFinish(false, reply->errorString());
        } else {
            if (onFinish)
                onFinish(true, QString());
        }
    });

    return reply;
}

QNetworkReply *NetworkClient::getJson(const QUrl &url,
                                       const QMap<QString, QString> &headers,
                                       Callback callback)
{
    QNetworkRequest req(url);
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        req.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    QNetworkReply *reply = m_nam->get(req);
    m_activeReplies.append(reply);

    connect(reply, &QNetworkReply::finished, this, [reply, callback, this]() {
        reply->deleteLater();
        m_activeReplies.removeAll(reply);
        if (reply->error() != QNetworkReply::NoError) {
            if (callback)
                callback(false, QByteArray(), reply->errorString());
        } else {
            if (callback)
                callback(true, reply->readAll(), QString());
        }
    });

    return reply;
}

void NetworkClient::cancelAll() {
    for (auto *r : m_activeReplies) {
        r->abort();
    }
    m_activeReplies.clear();
}
