#include "SetupController.h"
#include "Settings.h"
#include "../utils/Platform.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>

SetupController::SetupController(Settings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
    , m_nam(new QNetworkAccessManager(this))
    , m_fontsDir(Platform::fontsDir())
{
    m_steps = {
        {"deps",     QStringLiteral("Check system environment"), SetupStep::Pending, QString()},
        {"fonts",    QStringLiteral("Download fonts"),           SetupStep::Pending, QString()},
        {"ollama",   QStringLiteral("Detect Ollama service"),    SetupStep::Pending, QString()},
        {"llamacpp", QStringLiteral("Detect llama.cpp"),         SetupStep::Pending, QString()},
        {"finalize", QStringLiteral("Complete setup"),           SetupStep::Pending, QString()},
    };

    QString ghProxy = "https://ghproxy.vip/raw.githubusercontent.com/google/fonts/main";
    m_fontUrls = {
        ghProxy + "/ofl/outfit/Outfit%5Bwght%5D.ttf",
        ghProxy + "/ofl/notosanssc/NotoSansSC%5Bwght%5D.ttf",
        ghProxy + "/ofl/jetbrainsmono/JetBrainsMono%5Bwght%5D.ttf",
    };
}

QVariantList SetupController::stepsVariant() const {
    QVariantList list;
    for (const auto &s : m_steps) {
        QVariantMap m;
        m["id"] = s.id;
        m["name"] = s.name;
        m["status"] = static_cast<int>(s.status);
        m["detail"] = s.detail;
        list.append(m);
    }
    return list;
}

void SetupController::start() {
    if (m_running) return;

    m_running = true;
    m_complete = false;
    m_currentStepIdx = -1;
    m_progress = 0.0;
    m_fontDownloadIdx = 0;
    m_fontStepProgress = 0.0;
    m_currentText = QStringLiteral("Initializing...");
    emit isRunningChanged();
    emit currentStepTextChanged();

    QDir().mkpath(m_fontsDir);

    QTimer::singleShot(100, this, &SetupController::advanceStep);
}

void SetupController::retryStep(const QString &stepId) {
    for (int i = 0; i < m_steps.size(); ++i) {
        if (m_steps[i].id == stepId) {
            m_steps[i].status = SetupStep::Pending;
            m_steps[i].detail.clear();
            emit stepsChanged();

            m_currentStepIdx = i - 1;
            QTimer::singleShot(100, this, &SetupController::advanceStep);
            return;
        }
    }
}

void SetupController::advanceStep() {
    m_currentStepIdx++;
    if (m_currentStepIdx >= m_steps.size()) {
        m_running = false;
        m_complete = true;
        m_progress = 1.0;
        m_currentText = QStringLiteral("Setup complete");
        emit progressChanged();
        emit isCompleteChanged();
        emit isRunningChanged();
        emit currentStepTextChanged();
        return;
    }

    if (m_currentStepIdx > 0 && m_steps[m_currentStepIdx - 1].status == SetupStep::Failed) {
        // Previous step failed but user chose to continue
    }

    setStepStatus(m_steps[m_currentStepIdx].id, SetupStep::Running);
    updateProgress();

    const QString &id = m_steps[m_currentStepIdx].id;
    if (id == "deps")      runDependencies();
    else if (id == "fonts")    runFonts();
    else if (id == "ollama")   runOllama();
    else if (id == "llamacpp") runLlamaCpp();
    else if (id == "finalize") runFinalize();
}

void SetupController::runDependencies() {
    m_currentText = QStringLiteral("Checking system environment...");
    emit currentStepTextChanged();

    QString detail;

    // Check Qt
    QString qtVer = QString::fromLatin1(qVersion());
    detail += QStringLiteral("Qt %1").arg(qtVer);

    // Check disk space
    QString dataDir = Platform::dataDir();
    QDir().mkpath(dataDir);
    detail += QStringLiteral(", data dir ready");

    QTimer::singleShot(400, this, [this, detail]() {
        setStepStatus("deps", SetupStep::Done, detail);
        updateProgress();
        QTimer::singleShot(100, this, &SetupController::advanceStep);
    });
}

void SetupController::runFonts() {
    m_currentText = QStringLiteral("Downloading fonts...");
    emit currentStepTextChanged();
    m_fontDownloadIdx = 0;
    m_fontStepProgress = 0.0;
    setStepStatus("fonts", SetupStep::Running);
    downloadNextFont();
}

void SetupController::downloadNextFont() {
    if (m_fontDownloadIdx >= m_fontUrls.size()) {
        setStepStatus("fonts", SetupStep::Done, QStringLiteral("All fonts downloaded"));
        updateProgress();
        QTimer::singleShot(100, this, &SetupController::advanceStep);
        return;
    }

    QString url = m_fontUrls[m_fontDownloadIdx];
    QString filename;
    if (m_fontDownloadIdx == 0) filename = "Outfit[wght].ttf";
    else if (m_fontDownloadIdx == 1) filename = "NotoSansSC[wght].ttf";
    else filename = "JetBrainsMono[wght].ttf";

    QString savePath = m_fontsDir + "/" + filename;
    QFileInfo fi(savePath);
    if (fi.exists() && fi.size() > 10000) {
        setStepStatus("fonts", SetupStep::Running,
                      QStringLiteral("%1 (cached)").arg(filename));
        m_fontDownloadIdx++;
        m_fontStepProgress = static_cast<qreal>(m_fontDownloadIdx) / m_fontUrls.size();
        updateProgress();
        downloadNextFont();
        return;
    }

    m_currentText = QStringLiteral("Downloading %1...").arg(filename);
    emit currentStepTextChanged();

    QDir().mkpath(m_fontsDir);
    m_fontReply = m_nam->get(QNetworkRequest(QUrl(url)));

    connect(m_fontReply, &QNetworkReply::downloadProgress, this, &SetupController::onFontDownloadProgress);
    connect(m_fontReply, &QNetworkReply::finished, this, &SetupController::onFontDownloaded);
}

void SetupController::onFontDownloaded() {
    bool ok = m_fontReply->error() == QNetworkReply::NoError;
    QByteArray data = m_fontReply->readAll();
    m_fontReply->deleteLater();
    m_fontReply = nullptr;

    QString filename;
    if (m_fontDownloadIdx == 0) filename = "Outfit[wght].ttf";
    else if (m_fontDownloadIdx == 1) filename = "NotoSansSC[wght].ttf";
    else filename = "JetBrainsMono[wght].ttf";

    if (ok && data.size() > 10000) {
        QString savePath = m_fontsDir + "/" + filename;
        QFile f(savePath);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(data);
            f.close();
        }

        setStepStatus("fonts", SetupStep::Running,
                      QStringLiteral("%1 downloaded").arg(filename));
    } else {
        setStepStatus("fonts", SetupStep::Running,
                      QStringLiteral("%1 failed, skipping").arg(filename));
    }

    m_fontDownloadIdx++;
    m_fontStepProgress = static_cast<qreal>(m_fontDownloadIdx) / m_fontUrls.size();
    updateProgress();
    downloadNextFont();
}

void SetupController::onFontDownloadProgress(qint64 received, qint64 total) {
    if (total <= 0) return;

    qreal ratio = static_cast<qreal>(received) / total;
    QString filename;
    if (m_fontDownloadIdx == 0) filename = "Outfit";
    else if (m_fontDownloadIdx == 1) filename = "Noto Sans SC";
    else filename = "JetBrains Mono";

    setStepStatus("fonts", SetupStep::Running,
                  QStringLiteral("%1 (%2 / %3 MB)")
                      .arg(filename)
                      .arg(received / (1024.0 * 1024), 0, 'f', 1)
                      .arg(total / (1024.0 * 1024), 0, 'f', 1));

    m_fontStepProgress = (static_cast<qreal>(m_fontDownloadIdx) + ratio) / m_fontUrls.size();
    updateProgress();
}

void SetupController::runOllama() {
    m_currentText = QStringLiteral("Detecting Ollama service...");
    emit currentStepTextChanged();

    QUrl healthUrl("http://localhost:11434/api/tags"); QNetworkRequest req(healthUrl);
    req.setTransferTimeout(3000);

    QNetworkReply *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        bool ok = reply->error() == QNetworkReply::NoError;
        if (ok) {
            setStepStatus("ollama", SetupStep::Done, QStringLiteral("Ollama detected on port 11434"));
        } else {
            setStepStatus("ollama", SetupStep::Failed,
                          QStringLiteral("Not detected - install with scripts/setup-ollama.sh"));
        }
        updateProgress();
        QTimer::singleShot(100, this, &SetupController::advanceStep);
    });
}

void SetupController::runLlamaCpp() {
    m_currentText = QStringLiteral("Detecting llama.cpp...");
    emit currentStepTextChanged();

    QString serverPath = QStandardPaths::findExecutable("llama-server");
    if (serverPath.isEmpty()) {
        serverPath = QStandardPaths::findExecutable("llama-server",
            {QDir::homePath() + "/.local/bin"});
    }

    QString modelsDir = Platform::modelsDir();
    QDir md(modelsDir);
    QStringList ggufFiles = md.entryList({"*.gguf"}, QDir::Files);

    QString detail;
    SetupStep::Status status;

    if (!serverPath.isEmpty()) {
        detail = QStringLiteral("llama-server found");
        if (!ggufFiles.isEmpty()) {
            detail += QStringLiteral(", %1 model(s) available").arg(ggufFiles.size());
        } else {
            detail += QStringLiteral(" (no models yet)");
        }
        status = SetupStep::Done;
    } else {
        detail = QStringLiteral("Not found - install with scripts/setup-llama-cpp.sh");
        status = SetupStep::Failed;
    }

    QTimer::singleShot(300, this, [this, status, detail]() {
        setStepStatus("llamacpp", status, detail);
        updateProgress();
        QTimer::singleShot(100, this, &SetupController::advanceStep);
    });
}

void SetupController::runFinalize() {
    m_currentText = QStringLiteral("Finalizing setup...");
    emit currentStepTextChanged();

    m_settings->setConfigured(true);

    QTimer::singleShot(300, this, [this]() {
        setStepStatus("finalize", SetupStep::Done, QStringLiteral("Ready"));
        updateProgress();
        QTimer::singleShot(100, this, &SetupController::advanceStep);
    });
}

void SetupController::setStepStatus(const QString &id, SetupStep::Status status, const QString &detail) {
    for (auto &s : m_steps) {
        if (s.id == id) {
            s.status = status;
            s.detail = detail;
            emit stepsChanged();
            return;
        }
    }
}

void SetupController::updateProgress() {
    if (m_steps.isEmpty()) return;

    qreal completed = static_cast<qreal>(m_currentStepIdx) / TOTAL_STEPS;

    if (m_currentStepIdx >= 0 && m_currentStepIdx < m_steps.size()) {
        const QString &id = m_steps[m_currentStepIdx].id;
        if (id == "fonts") {
            completed += m_fontStepProgress / TOTAL_STEPS;
        } else if (m_steps[m_currentStepIdx].status == SetupStep::Done) {
            completed += 1.0 / TOTAL_STEPS;
        } else {
            completed += 0.3 / TOTAL_STEPS;
        }
    }

    m_progress = qBound(0.0, completed, 1.0);
    emit progressChanged();
}
