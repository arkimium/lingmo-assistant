#include "ChatController.h"
#include "Settings.h"
#include <QDateTime>
#include <QUuid>
#include "../providers/ProviderManager.h"
#include "../providers/AIProvider.h"
#include "../utils/Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ChatController::ChatController(Settings *settings, ProviderManager *providerMgr, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
    , m_providerMgr(providerMgr)
    , m_contextMgr(128000, this)
{
    m_contextMgr.setHistoryTokenLimit(settings->historyTokenLimit());

    connect(settings, &Settings::historyTokenLimitChanged, this, [this]() {
        m_contextMgr.setHistoryTokenLimit(m_settings->historyTokenLimit());
    });

    connect(&m_contextMgr, &ContextManager::budgetWarning, this, [this](int used, int budget) {
        emit contextWarning(QString("Context at %1%: %2/%3 tokens used")
                            .arg(static_cast<int>(m_contextMgr.utilizationPercent()))
                            .arg(used).arg(budget));
    });

    loadConversations();
    createNewConversation();
}

QVariantList ChatController::conversationsVariant() const {
    QVariantList list;
    for (auto *c : m_conversations) {
        QVariantMap m;
        m["id"] = c->id();
        m["title"] = c->title();
        m["messageCount"] = c->messageCount();
        m["providerId"] = c->providerId();
        list.append(m);
    }
    return list;
}

QString ChatController::activeConversationId() const {
    return m_activeConv ? m_activeConv->id() : QString();
}

void ChatController::sendMessage(const QString &text) {
    if (text.trimmed().isEmpty() || m_isStreaming) return;

    if (!m_activeConv) {
        createNewConversation();
    }

    auto *provider = m_providerMgr->provider(m_settings->activeProvider());
    if (!provider) {
        emit errorOccurred("No provider selected");
        return;
    }

    m_currentProvider = provider;
    connectProvider(provider);

    if (!provider->isAvailable()) {
        emit errorOccurred(provider->displayName() + " is not available");
        return;
    }

    m_activeConv->setProviderId(m_settings->activeProvider());
    m_activeConv->setModelName(m_settings->activeModel());

    m_contextMgr.setModelLimit(provider->contextLimit());
    m_contextMgr.setOutputReserve(m_settings->maxOutputTokens());

    auto apiReq = m_contextMgr.buildRequest(text);

    m_contextMgr.addMessage(Message::User, text);
    m_activeConv->addMessage(m_contextMgr.history().last());

    if (m_activeConv->messageCount() == 1) {
        QString title = text.left(40);
        if (text.length() > 40) title += "...";
        m_activeConv->setTitle(title);
    }

    QJsonArray messages;
    for (const auto &msg : apiReq.messages) {
        QJsonObject m;
        m["role"] = Message::roleToString(msg.role);
        m["content"] = msg.content;
        messages.append(m);
    }

    QJsonObject params;
    params["temperature"] = m_settings->temperature();
    params["max_tokens"] = m_settings->maxOutputTokens();
    params["stream"] = m_settings->streamEnabled();

    provider->sendMessage(m_activeConv->id(), messages, params);

    emit conversationsChanged();
}

void ChatController::cancelRequest() {
    if (m_currentProvider) {
        m_currentProvider->cancelRequest();
        disconnectProvider(m_currentProvider);
        m_currentProvider = nullptr;
    }
    m_isStreaming = false;
    m_streamBuffer.clear();
    emit streamingChanged();
}

void ChatController::createConversation() {
    createNewConversation();
    emit conversationsChanged();
}

void ChatController::switchConversation(const QString &id) {
    for (auto *c : m_conversations) {
        if (c->id() == id) {
            m_activeConv = c;
            m_contextMgr.clearHistory();
            for (const auto &msg : c->messages()) {
                m_contextMgr.addMessage(msg);
            }
            emit activeConversationChanged();
            return;
        }
    }
}

void ChatController::deleteConversation(const QString &id) {
    for (int i = 0; i < m_conversations.size(); ++i) {
        if (m_conversations[i]->id() == id) {
            if (m_activeConv == m_conversations[i]) {
                m_activeConv = nullptr;
                m_contextMgr.clearHistory();
            }
            delete m_conversations.takeAt(i);
            saveConversations();
            if (!m_activeConv) {
                createNewConversation();
            }
            emit conversationsChanged();
            return;
        }
    }
}

void ChatController::clearCurrentConversation() {
    if (m_activeConv) {
        m_activeConv->clearMessages();
        m_contextMgr.clearHistory();
        emit conversationsChanged();
    }
}

QVariantMap ChatController::getConversation(const QString &id) const {
    for (auto *c : m_conversations) {
        if (c->id() == id) {
            QVariantMap m;
            m["id"] = c->id();
            m["title"] = c->title();
            m["providerId"] = c->providerId();
            QJsonArray msgs;
            for (const auto &msg : c->messages()) {
                QJsonObject obj;
                obj["role"] = Message::roleToString(msg.role);
                obj["content"] = msg.content;
                msgs.append(obj);
            }
            m["messages"] = QJsonDocument(msgs).toVariant();
            return m;
        }
    }
    return {};
}

void ChatController::onStreamingStarted() {
    m_isStreaming = true;
    m_streamBuffer.clear();
    emit streamingChanged();
}

void ChatController::onTokenReceived(const QString &token) {
    m_streamBuffer += token;
    emit tokenReceived(token);
}

void ChatController::onStreamingFinished(const QString &fullResponse, const QJsonObject &usage) {
    m_isStreaming = false;
    m_streamBuffer = fullResponse;

    Message assistantMsg;
    assistantMsg.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    assistantMsg.role = Message::Assistant;
    assistantMsg.content = fullResponse;
    assistantMsg.timestamp = QDateTime::currentDateTime();
    assistantMsg.tokenCount = usage["completion_tokens"].toInt();

    m_contextMgr.addMessage(assistantMsg);
    if (m_activeConv) {
        m_activeConv->addMessage(assistantMsg);
    }

    emit streamingChanged();
    emit responseFinished(fullResponse);
    saveConversations();

    disconnectProvider(m_currentProvider);
    m_currentProvider = nullptr;
}

void ChatController::onProviderError(const QString &error) {
    m_isStreaming = false;
    emit streamingChanged();
    emit errorOccurred(error);
    disconnectProvider(m_currentProvider);
    m_currentProvider = nullptr;
}

void ChatController::loadConversations() {
    QJsonArray arr = m_settings->loadConversations();
    for (const auto &v : arr) {
        m_conversations.append(Conversation::fromJson(v.toObject(), this));
    }
}

void ChatController::saveConversations() {
    QJsonArray arr;
    for (auto *c : m_conversations) {
        arr.append(c->toJson());
    }
    m_settings->saveConversations(arr);
}

Conversation *ChatController::createNewConversation() {
    auto *conv = new Conversation(this);
    conv->setProviderId(m_settings->activeProvider());
    conv->setModelName(m_settings->activeModel());
    m_conversations.append(conv);
    m_activeConv = conv;
    m_contextMgr.clearHistory();
    emit activeConversationChanged();
    return conv;
}

void ChatController::connectProvider(AIProvider *provider) {
    connect(provider, &AIProvider::streamingStarted, this, &ChatController::onStreamingStarted);
    connect(provider, &AIProvider::tokenReceived, this, &ChatController::onTokenReceived);
    connect(provider, &AIProvider::streamingFinished, this, &ChatController::onStreamingFinished);
    connect(provider, &AIProvider::errorOccurred, this, &ChatController::onProviderError);
}

void ChatController::disconnectProvider(AIProvider *provider) {
    disconnect(provider, &AIProvider::streamingStarted, this, &ChatController::onStreamingStarted);
    disconnect(provider, &AIProvider::tokenReceived, this, &ChatController::onTokenReceived);
    disconnect(provider, &AIProvider::streamingFinished, this, &ChatController::onStreamingFinished);
    disconnect(provider, &AIProvider::errorOccurred, this, &ChatController::onProviderError);
}
