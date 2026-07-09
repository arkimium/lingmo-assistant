#pragma once
#include <QObject>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include "Message.h"

class Conversation : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int messageCount READ messageCount NOTIFY messagesChanged)
    Q_PROPERTY(QString providerId READ providerId WRITE setProviderId NOTIFY providerChanged)
    Q_PROPERTY(QString modelName READ modelName WRITE setModelName NOTIFY modelChanged)
    Q_PROPERTY(int totalTokens READ totalTokens NOTIFY messagesChanged)

public:
    explicit Conversation(QObject *parent = nullptr);
    explicit Conversation(const QString &convId, QObject *parent = nullptr);

    QString id() const { return m_id; }
    QString title() const { return m_title; }
    void setTitle(const QString &title);
    int messageCount() const;
    QString providerId() const { return m_providerId; }
    void setProviderId(const QString &id);
    QString modelName() const { return m_modelName; }
    void setModelName(const QString &name);
    int totalTokens() const;

    void addMessage(const Message &msg);
    void updateLastMessage(const QString &content);
    Message lastMessage() const;
    QList<Message> messages() const;
    QList<Message> messagesForApi() const;
    void setSystemPrompt(const QString &prompt);
    QString systemPrompt() const { return m_systemPrompt; }
    void clearMessages();
    void trimToTokens(int maxTokens);

    QJsonObject toJson() const;
    static Conversation *fromJson(const QJsonObject &obj, QObject *parent = nullptr);

signals:
    void titleChanged();
    void messagesChanged();
    void providerChanged();
    void modelChanged();

private:
    QString m_id;
    QString m_title;
    QString m_providerId;
    QString m_modelName;
    QString m_systemPrompt;
    QList<Message> m_messages;
};
