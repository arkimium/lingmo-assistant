#pragma once
#include <QObject>
#include <QByteArray>
#include <QJsonObject>
#include <QString>
#include <functional>

class StreamParser : public QObject {
    Q_OBJECT
public:
    enum class Format { SSE_OpenAI, SSE_Anthropic, JSON_Lines };

    explicit StreamParser(Format format = Format::SSE_OpenAI, QObject *parent = nullptr);

    using TokenCallback = std::function<void(const QString &token)>;
    using FinishCallback = std::function<void(const QString &fullText, const QJsonObject &usage)>;
    using ErrorCallback = std::function<void(const QString &error)>;

    void setCallbacks(TokenCallback onToken, FinishCallback onFinish, ErrorCallback onError);

    void feed(const QByteArray &data);
    void reset();
    QString accumulatedText() const { return m_fullText; }
    QJsonObject lastUsage() const { return m_usage; }

private:
    Format m_format;
    QByteArray m_buffer;
    QString m_fullText;
    QJsonObject m_usage;
    TokenCallback m_onToken;
    FinishCallback m_onFinish;
    ErrorCallback m_onError;

    void parseSSE(const QByteArray &payload);
    void parseJSONLine(const QByteArray &line);
    void parseAnthropicEvent(const QString &eventType, const QByteArray &data);
};
