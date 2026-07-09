#pragma once
#include "AIProvider.h"
#include "../network/NetworkClient.h"
#include "../network/StreamParser.h"
#include <QProcess>
#include <QTimer>
#include <memory>

class LlamaCppProvider : public AIProvider {
    Q_OBJECT
    Q_PROPERTY(QString serverState READ serverState NOTIFY serverStateChanged)
    Q_PROPERTY(int serverPort READ serverPort NOTIFY serverPortChanged)

public:
    explicit LlamaCppProvider(const QString &providerId,
                               const QString &name,
                               const QString &desc,
                               bool needsKey,
                               bool local,
                               QObject *parent = nullptr);

    QString id() const override { return m_id; }
    QString displayName() const override { return m_name; }
    QString description() const override { return m_desc; }
    bool requiresApiKey() const override { return false; }
    bool isLocal() const override { return true; }
    int contextLimit() const override { return m_ctxSize; }
    QList<QString> defaultModels() const override { return m_models; }
    bool isAvailable() const override;

    QString serverState() const { return m_serverState; }
    int serverPort() const { return m_port; }

    void setServerBinary(const QString &path) { m_serverBinary = path; }
    void setModelPath(const QString &path);
    void setCtxSize(int size) { m_ctxSize = size; }
    void setThreads(int n) { m_threads = n; }
    void setGpuLayers(int n) { m_gpuLayers = n; }

    Q_INVOKABLE bool startServer();
    Q_INVOKABLE void stopServer();
    Q_INVOKABLE bool isServerRunning() const;

    void fetchModels() override;
    void sendMessage(const QString &conversationId,
                     const QJsonArray &messages,
                     const QJsonObject &params) override;
    void cancelRequest() override;

signals:
    void serverStateChanged();
    void serverPortChanged();
    void serverLog(const QString &message);

private slots:
    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onReadyRead();
    void checkHealth();

private:
    QString m_id;
    QString m_name;
    QString m_desc;
    bool m_requiresKey;
    bool m_local;
    QString m_serverBinary = "llama-server";
    QString m_modelPath;
    int m_ctxSize = 4096;
    int m_threads = 4;
    int m_gpuLayers = 0;
    int m_port = 8081;
    int m_batchSize = 512;
    QList<QString> m_models;

    QProcess *m_process = nullptr;
    QTimer *m_healthTimer = nullptr;
    QTimer *m_killTimer = nullptr;
    QString m_serverState = "stopped";
    NetworkClient m_network;
    QNetworkReply *m_activeReply = nullptr;
    std::unique_ptr<StreamParser> m_parser;

    void setState(const QString &state);
    int findFreePort();
};
