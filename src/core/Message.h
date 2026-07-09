#pragma once
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

struct Message {
    enum Role { System, User, Assistant };

    QString id;
    Role role = User;
    QString content;
    QDateTime timestamp;
    int tokenCount = 0;
    bool isStreaming = false;

    static Message fromJson(const QJsonObject &obj);
    QJsonObject toJson() const;
    static QString roleToString(Role r);
    static Role roleFromString(const QString &s);
};
