#pragma once
#include <QList>
#include <QSet>
#include <QString>
#include "Message.h"

class ITokenEstimator;

struct TrimResult {
    QList<Message> included;
    QList<Message> excluded;
    int tokenCount = 0;
    bool wasTrimmed = false;
};

class ITrimmingStrategy {
public:
    virtual ~ITrimmingStrategy() = default;
    virtual TrimResult trim(const QList<Message> &allMessages,
                            const QString &systemPrompt,
                            int tokenBudget,
                            const ITokenEstimator &estimator) = 0;
};

class TokenBudgetStrategy : public ITrimmingStrategy {
public:
    TokenBudgetStrategy(int minRecent = 3);

    TrimResult trim(const QList<Message> &allMessages,
                    const QString &systemPrompt,
                    int tokenBudget,
                    const ITokenEstimator &estimator) override;

    void setMinRecent(int n) { m_minRecent = n; }

private:
    int m_minRecent;
};
