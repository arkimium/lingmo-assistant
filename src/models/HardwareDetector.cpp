#include "HardwareDetector.h"
#include <QThread>
#include <QProcess>
#include <QFile>
#include <QRegularExpression>
#include <QVariantMap>

int HardwareProfile::recommendedCtxSize() const {
    if (availRamBytes > 16LL * 1024 * 1024 * 1024) return 8192;
    if (availRamBytes > 8LL * 1024 * 1024 * 1024)  return 4096;
    return 2048;
}

int HardwareProfile::recommendedNGL() const {
    if (!hasGPU || vramFreeBytes <= 0) return 0;
    return 99;
}

int HardwareProfile::recommendedBatchSize() const {
    if (physicalCores >= 8) return 512;
    return 256;
}

HardwareDetector::HardwareDetector(QObject *parent) : QObject(parent) {}

HardwareProfile HardwareDetector::detect() {
    HardwareProfile p;
    p.physicalCores = detectCPUcores();
    p.totalRamBytes = detectTotalRam();
    p.availRamBytes = detectAvailRam();
    detectGPU(p);
    return p;
}

QVariantMap HardwareDetector::detectMap() {
    auto p = detect();
    QVariantMap m;
    m["physicalCores"] = p.physicalCores;
    m["totalRamGB"] = p.totalRamBytes / (1024.0 * 1024 * 1024);
    m["availRamGB"] = p.availRamBytes / (1024.0 * 1024 * 1024);
    m["hasGPU"] = p.hasGPU;
    m["gpuName"] = p.gpuName;
    m["vramTotalGB"] = p.vramTotalBytes / (1024.0 * 1024 * 1024);
    m["vramFreeGB"] = p.vramFreeBytes / (1024.0 * 1024 * 1024);
    m["recommendedThreads"] = p.recommendedThreads();
    m["recommendedCtxSize"] = p.recommendedCtxSize();
    m["recommendedNGL"] = p.recommendedNGL();
    return m;
}

int HardwareDetector::detectCPUcores() {
    return QThread::idealThreadCount();
}

int64_t HardwareDetector::detectTotalRam() {
    QFile f("/proc/meminfo");
    if (!f.open(QIODevice::ReadOnly)) return 0;
    QString content = f.readAll();
    for (const auto &line : content.split('\n')) {
        if (line.startsWith("MemTotal:")) {
            auto parts = line.split(QRegularExpression("\\s+"));
            if (parts.size() >= 2) {
                return parts[1].toLongLong() * 1024;
            }
        }
    }
    return 0;
}

int64_t HardwareDetector::detectAvailRam() {
    QFile f("/proc/meminfo");
    if (!f.open(QIODevice::ReadOnly)) return 0;
    QString content = f.readAll();
    for (const auto &line : content.split('\n')) {
        if (line.startsWith("MemAvailable:")) {
            auto parts = line.split(QRegularExpression("\\s+"));
            if (parts.size() >= 2) {
                return parts[1].toLongLong() * 1024;
            }
        }
    }
    return 0;
}

bool HardwareDetector::detectGPU(HardwareProfile &profile) {
    QProcess proc;
    proc.start("nvidia-smi", {"--query-gpu=name,memory.total,memory.free",
                               "--format=csv,noheader,nounits"});
    proc.waitForFinished(3000);
    if (proc.exitCode() == 0) {
        QString output = proc.readAllStandardOutput();
        auto parts = output.split(',');
        if (parts.size() >= 3) {
            profile.hasGPU = true;
            profile.gpuName = parts[0].trimmed();
            profile.vramTotalBytes = parts[1].trimmed().toLongLong() * 1024 * 1024;
            profile.vramFreeBytes = parts[2].trimmed().toLongLong() * 1024 * 1024;
            return true;
        }
    }
    return false;
}

int64_t HardwareDetector::detectVRamTotal() { HardwareProfile p; detectGPU(p); return p.vramTotalBytes; }
int64_t HardwareDetector::detectVRamFree()  { HardwareProfile p; detectGPU(p); return p.vramFreeBytes; }
