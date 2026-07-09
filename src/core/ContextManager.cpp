#include "ContextManager.h"
#include <QDateTime>
#include <QUuid>
#include <algorithm>

ContextManager::ContextManager(int modelContextLimit, QObject *parent)
    : QObject(parent)
    , m_modelLimit(modelContextLimit)
    , m_estimator(std::make_unique<CharTokenEstimator>())
    , m_strategy(std::make_unique<TokenBudgetStrategy>(3))
{
}

void ContextManager::setModelLimit(int limit) {
    m_modelLimit = limit;
    invalidateCache();
    emit statsChanged();
}

void ContextManager::setSafetyFactor(double factor) {
    m_safetyFactor = factor;
    invalidateCache();
    emit statsChanged();
}

void ContextManager::setOutputReserve(int tokens) {
    m_outputReserve = tokens;
    invalidateCache();
    emit statsChanged();
}

void ContextManager::setHistoryTokenLimit(int limit) {
    m_historyTokenLimit = limit;
    trimHistory();
    invalidateCache();
    emit statsChanged();
}

void ContextManager::setSystemPrompt(const QString &prompt) {
    m_systemPrompt = prompt;
    invalidateCache();
}

void ContextManager::setEstimator(std::unique_ptr<ITokenEstimator> estimator) {
    m_estimator = std::move(estimator);
    invalidateCache();
}

void ContextManager::setStrategy(std::unique_ptr<ITrimmingStrategy> strategy) {
    m_strategy = std::move(strategy);
    invalidateCache();
}

void ContextManager::addMessage(Message::Role role, const QString &content) {
    Message msg;
    msg.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    msg.role = role;
    msg.content = content;
    msg.timestamp = QDateTime::currentDateTime();
    msg.tokenCount = m_estimator->estimate(content);
    addMessage(msg);
}

void ContextManager::addMessage(const Message &msg) {
    m_messages.append(msg);
    trimHistory();
    invalidateCache();
    emit statsChanged();
}

void ContextManager::clearHistory() {
    m_messages.clear();
    invalidateCache();
    emit statsChanged();
}

ContextManager::ApiRequest ContextManager::buildRequest(const QString &newUserMessage) {
    Message userMsg;
    userMsg.id = "current_user";
    userMsg.role = Message::User;
    userMsg.content = newUserMessage;
    userMsg.tokenCount = m_estimator->estimate(newUserMessage);

    QList<Message> allMessages = m_messages;
    allMessages.append(userMsg);

    int budget = availableBudget();
    auto trimResult = m_strategy->trim(allMessages, m_systemPrompt, budget, *m_estimator);

    if (trimResult.wasTrimmed) {
        emit contextTrimmed(trimResult.excluded.size(),
                           m_estimator->estimateMessages(trimResult.excluded));
    }

    int used = m_estimator->estimate(m_systemPrompt) +
               m_estimator->estimateMessages(trimResult.included);

    double util = static_cast<double>(used) / budget;
    if (util > 0.85) {
        emit budgetWarning(used, budget);
    }

    ApiRequest req;
    if (!m_systemPrompt.isEmpty()) {
        Message sys;
        sys.role = Message::System;
        sys.content = m_systemPrompt;
        req.messages.append(sys);
    }
    req.messages.append(trimResult.included);
    req.estimatedInputTokens = used;
    return req;
}

int ContextManager::totalTokens() const {
    if (m_cachedTotal < 0) {
        const_cast<ContextManager*>(this)->m_cachedTotal = const_cast<ContextManager*>(this)->recalculateTokens();
    }
    return m_cachedTotal;
}

int ContextManager::remainingTokens() const {
    return std::max(0, m_historyTokenLimit - totalTokens());
}

double ContextManager::utilizationPercent() const {
    if (m_historyTokenLimit <= 0) return 0.0;
    return static_cast<double>(totalTokens()) / m_historyTokenLimit * 100.0;
}

bool ContextManager::isWarning() const {
    return utilizationPercent() > 85.0;
}

int ContextManager::availableBudget() const {
    int eff = static_cast<int>(m_modelLimit * m_safetyFactor);
    return std::max(0, eff - m_outputReserve);
}

void ContextManager::invalidateCache() {
    m_cachedTotal = -1;
}

int ContextManager::recalculateTokens() {
    int total = 0;
    for (const auto &m : m_messages) {
        total += m.tokenCount > 0 ? m.tokenCount : m_estimator->estimate(m.content);
    }
    return total;
}

void ContextManager::trimHistory() {
    if (m_messages.isEmpty() || m_historyTokenLimit <= 0) return;

    int budget = static_cast<int>(m_historyTokenLimit * m_safetyFactor);
    TrimResult trimResult = m_strategy->trim(m_messages, QString(), budget, *m_estimator);

    if (trimResult.wasTrimmed) {
        m_messages = trimResult.included;
        emit contextTrimmed(trimResult.excluded.size(),
                           m_estimator->estimateMessages(trimResult.excluded));
    }
}
