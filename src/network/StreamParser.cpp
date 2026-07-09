#include "StreamParser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

StreamParser::StreamParser(Format format, QObject *parent)
    : QObject(parent)
    , m_format(format)
{
}

void StreamParser::setCallbacks(TokenCallback onToken, FinishCallback onFinish, ErrorCallback onError) {
    m_onToken = onToken;
    m_onFinish = onFinish;
    m_onError = onError;
}

void StreamParser::feed(const QByteArray &data) {
    m_buffer.append(data);
    m_buffer.replace("\r\n", "\n");

    switch (m_format) {
    case Format::SSE_OpenAI: {
        int safety = 0;
        while (safety < 10000) {
            int idx = m_buffer.indexOf("\n\n");
            if (idx < 0) break;
            QByteArray chunk = m_buffer.left(idx);
            m_buffer.remove(0, idx + 2);
            if (chunk.isEmpty()) { safety++; continue; }
            QList<QByteArray> lines = chunk.split('\n');
            for (const auto &line : lines) {
                if (line.startsWith("data: ")) {
                    parseSSE(line.mid(6));
                }
            }
            safety++;
        }
        break;
    }
    case Format::SSE_Anthropic: {
        int safety = 0;
        while (safety < 10000) {
            int idx = m_buffer.indexOf("\n\n");
            if (idx < 0) break;
            QByteArray chunk = m_buffer.left(idx);
            m_buffer.remove(0, idx + 2);
            if (chunk.isEmpty()) { safety++; continue; }

            QList<QByteArray> lines = chunk.split('\n');
            QByteArray currentEvent;
            QByteArray currentData;
            for (const auto &line : lines) {
                if (line.startsWith("event: ")) {
                    currentEvent = line.mid(7);
                } else if (line.startsWith("data: ")) {
                    currentData = line.mid(6);
                }
            }
            if (!currentEvent.isEmpty() && !currentData.isEmpty()) {
                parseAnthropicEvent(QString::fromUtf8(currentEvent), currentData);
            }
            safety++;
        }
        break;
    }
    case Format::JSON_Lines: {
        int safety = 0;
        while (safety < 10000) {
            int idx = m_buffer.indexOf('\n');
            if (idx < 0) break;
            QByteArray line = m_buffer.left(idx).trimmed();
            m_buffer.remove(0, idx + 1);
            if (line.isEmpty()) { safety++; continue; }
            parseJSONLine(line);
            safety++;
        }
        break;
    }
    default: break;
    }
}

void StreamParser::parseSSE(const QByteArray &payload) {
    if (payload.trimmed() == "[DONE]") {
        if (m_onFinish) m_onFinish(m_fullText, m_usage);
        return;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(payload, &err);
    if (err.error != QJsonParseError::NoError) {
        if (m_onError) m_onError("SSE parse error: " + err.errorString());
        return;
    }

    QJsonObject obj = doc.object();
    QJsonArray choices = obj["choices"].toArray();
    if (choices.isEmpty()) return;

    QJsonObject choice = choices.first().toObject();
    QJsonObject delta = choice["delta"].toObject();

    QString content = delta["content"].toString();
    if (!content.isEmpty()) {
        m_fullText += content;
        if (m_onToken) m_onToken(content);
    }

    QString finishReason = choice["finish_reason"].toString();
    if (!finishReason.isEmpty() && finishReason != "null") {
        m_usage = obj["usage"].toObject();
        if (m_onFinish) m_onFinish(m_fullText, m_usage);
    }
}

void StreamParser::parseJSONLine(const QByteArray &line) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(line, &err);
    if (err.error != QJsonParseError::NoError) return;

    QJsonObject obj = doc.object();
    bool done = obj["done"].toBool(false);

    QJsonObject message = obj["message"].toObject();
    QString content = message["content"].toString();
    if (!content.isEmpty()) {
        m_fullText += content;
        if (m_onToken) m_onToken(content);
    }

    if (done) {
        QJsonObject usage;
        usage["prompt_tokens"] = obj["prompt_eval_count"].toInt();
        usage["completion_tokens"] = obj["eval_count"].toInt();
        if (m_onFinish) m_onFinish(m_fullText, m_usage);
    }
}

void StreamParser::parseAnthropicEvent(const QString &eventType, const QByteArray &data) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) return;

    QJsonObject obj = doc.object();

    if (eventType == "content_block_delta") {
        QJsonObject delta = obj["delta"].toObject();
        QString text = delta["text"].toString();
        if (!text.isEmpty()) {
            m_fullText += text;
            if (m_onToken) m_onToken(text);
        }
    } else if (eventType == "message_stop") {
        if (m_onFinish) m_onFinish(m_fullText, m_usage);
    }
}

void StreamParser::reset() {
    m_buffer.clear();
    m_fullText.clear();
    m_usage = QJsonObject();
}
