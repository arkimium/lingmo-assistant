#include "LlamaCppProvider.h"
#include "../utils/Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QTcpServer>

LlamaCppProvider::LlamaCppProvider(const QString &providerId,
                                     const QString &name,
                                     const QString &desc,
                                     bool needsKey,
                                     bool local,
                                     QObject *parent)
    : AIProvider(parent)
    , m_id(providerId)
    , m_name(name)
    , m_desc(desc)
    , m_requiresKey(needsKey)
    , m_local(local)
    , m_network(this)
{
}

void LlamaCppProvider::setModelPath(const QString &path) {
    m_modelPath = path;
    m_models = {QFileInfo(path).baseName()};
}

bool LlamaCppProvider::isAvailable() const {
    return !m_serverBinary.isEmpty() && !m_modelPath.isEmpty() && QFile::exists(m_modelPath);
}

bool LlamaCppProvider::isServerRunning() const {
    return m_serverState == "running";
}

bool LlamaCppProvider::startServer() {
    if (m_serverState != "stopped") return false;
    if (!isAvailable()) return false;

    m_port = findFreePort();

    m_process = new QProcess(this);
    QStringList args;
    args << "-m" << m_modelPath
         << "--host" << "127.0.0.1"
         << "--port" << QString::number(m_port)
         << "--ctx-size" << QString::number(m_ctxSize)
         << "--threads" << QString::number(m_threads)
         << "--n-gpu-layers" << QString::number(m_gpuLayers)
         << "--batch-size" << QString::number(m_batchSize)
         << "--parallel" << "1"
         << "--mlock";

    m_process->setProgram(m_serverBinary);
    m_process->setArguments(args);
    m_process->setProcessChannelMode(QProcess::SeparateChannels);

    connect(m_process, &QProcess::started, this, &LlamaCppProvider::onProcessStarted);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &LlamaCppProvider::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &LlamaCppProvider::onReadyRead);
    connect(m_process, &QProcess::readyReadStandardError, this, &LlamaCppProvider::onReadyRead);

    setState("starting");
    m_process->start();
    return true;
}

void LlamaCppProvider::stopServer() {
    if (!m_process) return;

    setState("stopping");

    if (m_killTimer) delete m_killTimer;
    m_killTimer = new QTimer(this);
    m_killTimer->setSingleShot(true);
    m_killTimer->start(10000);
    connect(m_killTimer, &QTimer::timeout, this, [this]() {
        if (m_process && m_process->state() != QProcess::NotRunning) {
            m_process->kill();
        }
    });

    m_process->terminate();
}

void LlamaCppProvider::fetchModels() {
    emit modelsReady(m_models);
}

void LlamaCppProvider::sendMessage(const QString &conversationId,
                                    const QJsonArray &messages,
                                    const QJsonObject &params)
{
    Q_UNUSED(conversationId);

    if (m_serverState != "running") {
        emit errorOccurred("llama-server is not running");
        return;
    }

    m_parser = std::make_unique<StreamParser>(StreamParser::Format::SSE_OpenAI);
    m_parser->setCallbacks(
        [this](const QString &token) { emit tokenReceived(token); },
        [this](const QString &fullText, const QJsonObject &usage) {
            emit streamingFinished(fullText, usage);
        },
        [this](const QString &error) { emit errorOccurred(error); }
    );

    QJsonObject body;
    body["model"] = m_models.isEmpty() ? "default" : m_models.first();
    body["messages"] = messages;
    body["stream"] = true;
    if (params.contains("temperature")) body["temperature"] = params["temperature"];

    QUrl url(QString("http://127.0.0.1:%1/v1/chat/completions").arg(m_port));
    QMap<QString, QString> headers;

    emit streamingStarted();

    m_activeReply = m_network.postJsonStream(url, body, headers,
        [this](const QByteArray &chunk) {
            if (m_parser) m_parser->feed(chunk);
        },
        [this](bool success, const QString &error) {
            m_activeReply = nullptr;
            if (!success) {
                emit errorOccurred(error);
            }
        });
}

void LlamaCppProvider::cancelRequest() {
    if (m_activeReply) {
        m_activeReply->abort();
        m_activeReply = nullptr;
    }
}

void LlamaCppProvider::onProcessStarted() {
    if (m_healthTimer) delete m_healthTimer;
    m_healthTimer = new QTimer(this);
    m_healthTimer->setInterval(500);
    connect(m_healthTimer, &QTimer::timeout, this, &LlamaCppProvider::checkHealth);
    m_healthTimer->start();
}

void LlamaCppProvider::onProcessFinished(int exitCode, QProcess::ExitStatus status) {
    Q_UNUSED(exitCode);
    Q_UNUSED(status);
    if (m_healthTimer) m_healthTimer->stop();
    if (m_killTimer) m_killTimer->stop();
    m_process->deleteLater();
    m_process = nullptr;
    setState("stopped");
}

void LlamaCppProvider::onReadyRead() {
    QByteArray data = m_process->readAllStandardOutput();
    if (data.isEmpty()) data = m_process->readAllStandardError();
    emit serverLog(QString::fromUtf8(data));
}

void LlamaCppProvider::checkHealth() {
    QUrl url(QString("http://127.0.0.1:%1/health").arg(m_port));
    m_network.getJson(url, {}, [this](bool success, const QByteArray &, const QString &) {
        if (success && m_serverState == "starting") {
            setState("running");
            if (m_healthTimer) m_healthTimer->stop();
        }
    });
}

void LlamaCppProvider::setState(const QString &state) {
    if (m_serverState != state) {
        m_serverState = state;
        emit serverStateChanged();
    }
}

int LlamaCppProvider::findFreePort() {
    QTcpServer server;
    server.listen(QHostAddress::LocalHost, 0);
    int port = server.serverPort();
    server.close();
    return port > 0 ? port : 8081;
}
