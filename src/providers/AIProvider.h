#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class AIProvider : public QObject {
    Q_OBJECT
public:
    explicit AIProvider(QObject *parent = nullptr) : QObject(parent) {}

    virtual QString id() const = 0;
    virtual QString displayName() const = 0;
    virtual QString description() const = 0;
    virtual bool requiresApiKey() const = 0;
    virtual bool isLocal() const = 0;
    virtual int contextLimit() const { return 128000; }
    virtual QList<QString> defaultModels() const = 0;
    virtual bool isAvailable() const { return true; }

    virtual void fetchModels() {}
    virtual void sendMessage(const QString &conversationId,
                             const QJsonArray &messages,
                             const QJsonObject &params) = 0;
    virtual void cancelRequest() = 0;

signals:
    void streamingStarted();
    void tokenReceived(const QString &token);
    void streamingFinished(const QString &fullResponse, const QJsonObject &usage);
    void errorOccurred(const QString &error);
    void modelsReady(const QList<QString> &models);
};
