#pragma once
#include <QObject>
#include <memory>
#include "Message.h"
#include "TokenEstimator.h"
#include "TrimmingStrategy.h"

class ContextManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(int totalTokens READ totalTokens NOTIFY statsChanged)
    Q_PROPERTY(int remainingTokens READ remainingTokens NOTIFY statsChanged)
    Q_PROPERTY(double utilizationPercent READ utilizationPercent NOTIFY statsChanged)
    Q_PROPERTY(bool isWarning READ isWarning NOTIFY statsChanged)

public:
    explicit ContextManager(int modelContextLimit = 128000, QObject *parent = nullptr);

    void setModelLimit(int limit);
    int modelLimit() const { return m_modelLimit; }

    void setSafetyFactor(double factor);
    double safetyFactor() const { return m_safetyFactor; }

    void setOutputReserve(int tokens);
    int outputReserve() const { return m_outputReserve; }

    void setHistoryTokenLimit(int limit);
    int historyTokenLimit() const { return m_historyTokenLimit; }

    void setSystemPrompt(const QString &prompt);
    QString systemPrompt() const { return m_systemPrompt; }

    void setEstimator(std::unique_ptr<ITokenEstimator> estimator);
    void setStrategy(std::unique_ptr<ITrimmingStrategy> strategy);

    void addMessage(Message::Role role, const QString &content);
    void addMessage(const Message &msg);
    void clearHistory();

    struct ApiRequest {
        QList<Message> messages;
        int estimatedInputTokens = 0;
    };

    ApiRequest buildRequest(const QString &newUserMessage);

    int totalTokens() const;
    int remainingTokens() const;
    double utilizationPercent() const;
    bool isWarning() const;

    int availableBudget() const;
    QList<Message> history() const { return m_messages; }

signals:
    void statsChanged();
    void contextTrimmed(int messagesRemoved, int tokensFreed);
    void budgetWarning(int tokensUsed, int budget);

private:
    int m_modelLimit;
    int m_historyTokenLimit = 8192;
    double m_safetyFactor = 0.85;
    int m_outputReserve = 0;
    QString m_systemPrompt;
    QList<Message> m_messages;
    std::unique_ptr<ITokenEstimator> m_estimator;
    std::unique_ptr<ITrimmingStrategy> m_strategy;
    int m_cachedTotal = -1;

    void invalidateCache();
    int recalculateTokens();
    void trimHistory();
};
