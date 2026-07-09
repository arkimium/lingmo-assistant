#pragma once
#include <QString>
#include <QList>
#include "Message.h"

class ITokenEstimator {
public:
    virtual ~ITokenEstimator() = default;
    virtual int estimate(const QString &text) const = 0;
    virtual int quickEstimate(const QString &text) const { return estimate(text); }

    int estimateMessages(const QList<Message> &messages) const {
        int total = 3;
        for (const auto &msg : messages) {
            total += 4 + estimate(msg.content);
        }
        return total;
    }
};

class CharTokenEstimator : public ITokenEstimator {
public:
    int estimate(const QString &text) const override;
    int quickEstimate(const QString &text) const override;
};
