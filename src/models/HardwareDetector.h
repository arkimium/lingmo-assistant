#pragma once
#include <QObject>
#include <QString>

struct HardwareProfile {
    int physicalCores = 4;
    int64_t totalRamBytes = 0;
    int64_t availRamBytes = 0;
    bool hasGPU = false;
    QString gpuName;
    int64_t vramTotalBytes = 0;
    int64_t vramFreeBytes = 0;

    int recommendedThreads() const { return std::max(1, physicalCores - 1); }
    int recommendedCtxSize() const;
    int recommendedNGL() const;
    int recommendedBatchSize() const;
};

class HardwareDetector : public QObject {
    Q_OBJECT
public:
    explicit HardwareDetector(QObject *parent = nullptr);

    HardwareProfile detect();
    Q_INVOKABLE QVariantMap detectMap();

private:
    int detectCPUcores();
    int64_t detectTotalRam();
    int64_t detectAvailRam();
    bool detectGPU(HardwareProfile &profile);
    int64_t detectVRamTotal();
    int64_t detectVRamFree();
};
