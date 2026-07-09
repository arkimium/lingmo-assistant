#include "TrimmingStrategy.h"
#include "TokenEstimator.h"

TokenBudgetStrategy::TokenBudgetStrategy(int minRecent)
    : m_minRecent(minRecent)
{
}

TrimResult TokenBudgetStrategy::trim(const QList<Message> &allMessages,
                                      const QString &systemPrompt,
                                      int tokenBudget,
                                      const ITokenEstimator &estimator)
{
    TrimResult result;
    int used = estimator.estimate(systemPrompt) + 3;

    if (allMessages.isEmpty()) {
        result.tokenCount = used;
        return result;
    }

    int recentStart = std::max<int>(0, static_cast<int>(allMessages.size() - m_minRecent));
    QList<Message> recent = allMessages.mid(recentStart);
    for (const auto &msg : recent) {
        used += 4 + estimator.estimate(msg.content);
    }

    for (int i = recentStart - 1; i >= 0; --i) {
        int msgTokens = 4 + estimator.estimate(allMessages[i].content);
        if (used + msgTokens <= tokenBudget) {
            used += msgTokens;
        } else {
            result.excluded.append(allMessages[i]);
        }
    }

    QSet<QString> excludedIds;
    for (const auto &m : result.excluded) {
        excludedIds.insert(m.id);
    }

    for (const auto &msg : allMessages) {
        if (!excludedIds.contains(msg.id)) {
            result.included.append(msg);
        }
    }

    result.tokenCount = used;
    result.wasTrimmed = !result.excluded.isEmpty();
    return result;
}
