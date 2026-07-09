#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QMap>
#include <functional>

class NetworkClient : public QObject {
    Q_OBJECT
public:
    explicit NetworkClient(QObject *parent = nullptr);
    ~NetworkClient() override;

    using Callback = std::function<void(bool success, const QByteArray &data, const QString &error)>;
    using StreamCallback = std::function<void(const QByteArray &chunk)>;
    using FinishCallback = std::function<void(bool success, const QString &error)>;

    QNetworkReply *postJson(const QUrl &url, const QJsonObject &body,
                            const QMap<QString, QString> &headers,
                            Callback callback);

    QNetworkReply *postJsonStream(const QUrl &url, const QJsonObject &body,
                                   const QMap<QString, QString> &headers,
                                   StreamCallback onChunk,
                                   FinishCallback onFinish);

    QNetworkReply *getJson(const QUrl &url, const QMap<QString, QString> &headers,
                           Callback callback);

    void cancelAll();

private:
    QNetworkAccessManager *m_nam;
    QList<QNetworkReply *> m_activeReplies;
};
