#include "ModelManager.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QVariantMap>
#include <QVariantList>

ModelManager::ModelManager(QObject *parent) : QObject(parent) {}

QString ModelManager::modelsDir() const {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/models";
    QDir().mkpath(dir);
    return dir;
}

QVariantList ModelManager::scanModels() const {
    QVariantList result;
    QDir dir(modelsDir());
    for (const auto &entry : dir.entryInfoList({"*.gguf"}, QDir::Files)) {
        auto info = extractInfo(entry.absoluteFilePath());
        QVariantMap m;
        m["filePath"] = info.filePath;
        m["name"] = info.name;
        m["architecture"] = info.architecture;
        m["fileSize"] = static_cast<qint64>(info.fileSize);
        m["quantization"] = info.quantization;
        m["contextLength"] = info.contextLength;
        m["totalLayers"] = info.totalLayers;
        result.append(m);
    }
    return result;
}

QVariantMap ModelManager::modelInfo(const QString &filePath) const {
    auto info = extractInfo(filePath);
    QVariantMap m;
    m["name"] = info.name;
    m["fileSizeMB"] = info.fileSize / (1024.0 * 1024);
    m["quantization"] = info.quantization;
    return m;
}

bool ModelManager::deleteModel(const QString &filePath) {
    QFile f(filePath);
    bool ok = f.remove();
    if (ok) emit modelsChanged();
    return ok;
}

ModelInfo ModelManager::extractInfo(const QString &path) const {
    ModelInfo info;
    info.filePath = path;
    info.name = QFileInfo(path).baseName();

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return info;

    QByteArray magic = f.read(4);
    if (magic != QByteArrayLiteral("GGUF")) {
        info.name = "Invalid GGUF";
        return info;
    }

    info.fileSize = f.size();

    QByteArray header;
    header.append(magic);
    header.append(f.read(4));
    QByteArray tensorCountBytes = f.read(8);
    QByteArray kvCountBytes = f.read(8);
    header.append(tensorCountBytes);
    header.append(kvCountBytes);

    uint64_t kvCount;
    memcpy(&kvCount, kvCountBytes.constData(), 8);

    if (kvCount > 1000) {
        f.close();
        return info;
    }

    std::function<QVariant(QByteArray&, int&)> readTypeAndValue; readTypeAndValue = [&](QByteArray &buf, int &pos) -> QVariant {
        if (pos + 4 > buf.size()) return {};
        uint32_t type;
        memcpy(&type, buf.constData() + pos, 4);
        pos += 4;
        switch (type) {
        case 0: { uint8_t v; if (pos + 1 > buf.size()) return {}; memcpy(&v, buf.constData() + pos, 1); pos += 1; return QVariant::fromValue(static_cast<bool>(v)); }
        case 1: { int8_t v; if (pos + 1 > buf.size()) return {}; memcpy(&v, buf.constData() + pos, 1); pos += 1; return v; }
        case 2: { uint16_t v; if (pos + 2 > buf.size()) return {}; memcpy(&v, buf.constData() + pos, 2); pos += 2; return v; }
        case 3: { int16_t v; if (pos + 2 > buf.size()) return {}; memcpy(&v, buf.constData() + pos, 2); pos += 2; return v; }
        case 4: case 5: { uint32_t v; if (pos + 4 > buf.size()) return {}; memcpy(&v, buf.constData() + pos, 4); pos += 4; return v; }
        case 6: case 7: { uint64_t v; if (pos + 8 > buf.size()) return {}; memcpy(&v, buf.constData() + pos, 8); pos += 8; return static_cast<qint64>(v); }
        case 8: case 9: case 10: { double v; if (pos + 8 > buf.size()) return {}; memcpy(&v, buf.constData() + pos, 8); pos += 8; return v; }
        case 11: { uint64_t len; if (pos + 8 > buf.size()) return {}; memcpy(&len, buf.constData() + pos, 8); pos += 8; if (pos + static_cast<int>(len) > buf.size()) return {}; QString s = QString::fromUtf8(buf.constData() + pos, static_cast<int>(len)); pos += static_cast<int>(len); return s; }
        case 12: { uint64_t len; memcpy(&len, buf.constData() + pos, 8); pos += 8; pos += static_cast<int>(len); return QVariant(); }
        case 13: { uint64_t count; memcpy(&count, buf.constData() + pos, 8); pos += 8; for (uint64_t i = 0; i < count && i < 100; i++) readTypeAndValue(buf, pos); return QVariant(); }
        default: return {};
        }
    };

    QByteArray metaBytes = f.readAll();
    QByteArray fullBuffer = header + metaBytes;
    int metaPos = header.size();

    for (uint64_t i = 0; i < kvCount; i++) {
        if (metaPos + 8 > fullBuffer.size()) break;
        uint64_t keyLen;
        memcpy(&keyLen, fullBuffer.constData() + metaPos, 8);
        metaPos += 8;
        if (metaPos + static_cast<int>(keyLen) > fullBuffer.size()) break;
        QString key = QString::fromUtf8(fullBuffer.constData() + metaPos, static_cast<int>(keyLen));
        metaPos += static_cast<int>(keyLen);

        if (key == "general.architecture") {
            info.architecture = readTypeAndValue(fullBuffer, metaPos).toString();
        } else if (key == "general.file_type") {
            uint32_t ft = readTypeAndValue(fullBuffer, metaPos).toUInt();
            switch (ft) {
            case 2: case 3: case 7: info.quantization = "Q4_K_M"; break;
            case 8: info.quantization = "Q5_K_M"; break;
            case 10: info.quantization = "Q6_K"; break;
            case 12: info.quantization = "Q8_0"; break;
            case 1: info.quantization = "F16"; break;
            default: info.quantization = QString("Q%1").arg(ft); break;
            }
        } else if (key.contains("context_length")) {
            info.contextLength = readTypeAndValue(fullBuffer, metaPos).toInt();
        } else if (key.contains("block_count")) {
            info.totalLayers = readTypeAndValue(fullBuffer, metaPos).toInt();
        } else {
            readTypeAndValue(fullBuffer, metaPos);
        }
    }

    f.close();
    return info;
}
