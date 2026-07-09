#include "Conversation.h"
#include <QDateTime>
#include <QUuid>
#include <QJsonArray>

Message Message::fromJson(const QJsonObject &obj) {
    Message m;
    m.id = obj["id"].toString();
    m.role = roleFromString(obj["role"].toString());
    m.content = obj["content"].toString();
    m.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
    m.tokenCount = obj["tokenCount"].toInt();
    return m;
}

QJsonObject Message::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["role"] = roleToString(role);
    obj["content"] = content;
    obj["timestamp"] = timestamp.toString(Qt::ISODate);
    obj["tokenCount"] = tokenCount;
    return obj;
}

QString Message::roleToString(Message::Role r) {
    switch (r) {
    case System:    return "system";
    case Assistant: return "assistant";
    default:        return "user";
    }
}

Message::Role Message::roleFromString(const QString &s) {
    if (s == "system")    return System;
    if (s == "assistant") return Assistant;
    return User;
}

Conversation::Conversation(QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

Conversation::Conversation(const QString &convId, QObject *parent)
    : QObject(parent)
    , m_id(convId)
{
}

void Conversation::setTitle(const QString &title) {
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

void Conversation::setProviderId(const QString &id) {
    if (m_providerId != id) {
        m_providerId = id;
        emit providerChanged();
    }
}

void Conversation::setModelName(const QString &name) {
    if (m_modelName != name) {
        m_modelName = name;
        emit modelChanged();
    }
}

int Conversation::messageCount() const {
    return m_messages.size();
}

int Conversation::totalTokens() const {
    int total = 0;
    for (const auto &m : m_messages) {
        total += m.tokenCount;
    }
    return total;
}

void Conversation::addMessage(const Message &msg) {
    m_messages.append(msg);
    emit messagesChanged();
}

void Conversation::updateLastMessage(const QString &content) {
    if (!m_messages.isEmpty()) {
        m_messages.last().content = content;
        emit messagesChanged();
    }
}

Message Conversation::lastMessage() const {
    return m_messages.isEmpty() ? Message() : m_messages.last();
}

QList<Message> Conversation::messages() const {
    return m_messages;
}

QList<Message> Conversation::messagesForApi() const {
    QList<Message> result;
    if (!m_systemPrompt.isEmpty()) {
        Message sys;
        sys.role = Message::System;
        sys.content = m_systemPrompt;
        result.append(sys);
    }
    result.append(m_messages);
    return result;
}

void Conversation::setSystemPrompt(const QString &prompt) {
    m_systemPrompt = prompt;
}

void Conversation::clearMessages() {
    m_messages.clear();
    emit messagesChanged();
}

void Conversation::trimToTokens(int maxTokens) {
    int total = 0;
    QList<Message> kept;
    for (int i = m_messages.size() - 1; i >= 0; --i) {
        int t = m_messages[i].tokenCount > 0 ? m_messages[i].tokenCount : m_messages[i].content.length() / 3;
        if (total + t <= maxTokens) {
            kept.prepend(m_messages[i]);
            total += t;
        } else {
            break;
        }
    }
    if (kept.size() != m_messages.size()) {
        m_messages = kept;
        emit messagesChanged();
    }
}

QJsonObject Conversation::toJson() const {
    QJsonObject obj;
    obj["id"] = m_id;
    obj["title"] = m_title;
    obj["providerId"] = m_providerId;
    obj["modelName"] = m_modelName;
    obj["systemPrompt"] = m_systemPrompt;
    QJsonArray msgs;
    for (const auto &m : m_messages) {
        msgs.append(m.toJson());
    }
    obj["messages"] = msgs;
    return obj;
}

Conversation *Conversation::fromJson(const QJsonObject &obj, QObject *parent) {
    auto *conv = new Conversation(obj["id"].toString(), parent);
    conv->setTitle(obj["title"].toString());
    conv->setProviderId(obj["providerId"].toString());
    conv->setModelName(obj["modelName"].toString());
    conv->setSystemPrompt(obj["systemPrompt"].toString());
    for (const auto &v : obj["messages"].toArray()) {
        conv->addMessage(Message::fromJson(v.toObject()));
    }
    return conv;
}
