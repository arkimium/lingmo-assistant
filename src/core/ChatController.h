#pragma once
#include <QObject>
#include <QList>
#include <QJsonArray>
#include <memory>
#include "Conversation.h"
#include "ContextManager.h"

class Settings;
class ProviderManager;
class AIProvider;

class ChatController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentAssistantMessage READ currentAssistantMessage NOTIFY streamingChanged)
    Q_PROPERTY(bool isStreaming READ isStreaming NOTIFY streamingChanged)
    Q_PROPERTY(QVariantList conversations READ conversationsVariant NOTIFY conversationsChanged)
    Q_PROPERTY(QString activeConversationId READ activeConversationId NOTIFY activeConversationChanged)
    Q_PROPERTY(ContextManager* contextManager READ contextManager CONSTANT)

public:
    explicit ChatController(Settings *settings, ProviderManager *providerMgr, QObject *parent = nullptr);

    QString currentAssistantMessage() const { return m_streamBuffer; }
    bool isStreaming() const { return m_isStreaming; }
    ContextManager *contextManager() { return &m_contextMgr; }

    QVariantList conversationsVariant() const;
    QString activeConversationId() const;

    Q_INVOKABLE void sendMessage(const QString &text);
    Q_INVOKABLE void cancelRequest();
    Q_INVOKABLE void createConversation();
    Q_INVOKABLE void switchConversation(const QString &id);
    Q_INVOKABLE void deleteConversation(const QString &id);
    Q_INVOKABLE void clearCurrentConversation();
    Q_INVOKABLE QVariantMap getConversation(const QString &id) const;

signals:
    void streamingChanged();
    void tokenReceived(const QString &token);
    void responseFinished(const QString &fullResponse);
    void errorOccurred(const QString &error);
    void conversationsChanged();
    void activeConversationChanged();
    void contextWarning(const QString &message);

private slots:
    void onStreamingStarted();
    void onTokenReceived(const QString &token);
    void onStreamingFinished(const QString &fullResponse, const QJsonObject &usage);
    void onProviderError(const QString &error);

private:
    Settings *m_settings;
    ProviderManager *m_providerMgr;
    ContextManager m_contextMgr;
    QList<Conversation *> m_conversations;
    Conversation *m_activeConv = nullptr;
    AIProvider *m_currentProvider = nullptr;
    QString m_streamBuffer;
    bool m_isStreaming = false;

    void loadConversations();
    void saveConversations();
    Conversation *createNewConversation();
    void connectProvider(AIProvider *provider);
    void disconnectProvider(AIProvider *provider);
};
