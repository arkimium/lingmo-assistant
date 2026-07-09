#pragma once
#include <QObject>
#include <QList>
#include <QString>

struct ModelInfo {
    QString filePath;
    QString name;
    QString architecture;
    int64_t fileSize;
    QString quantization;
    int contextLength;
    int totalLayers;
};

class ModelManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString modelsDir READ modelsDir CONSTANT)

public:
    explicit ModelManager(QObject *parent = nullptr);

    QString modelsDir() const;

    Q_INVOKABLE QVariantList scanModels() const;
    Q_INVOKABLE QVariantMap modelInfo(const QString &filePath) const;
    Q_INVOKABLE bool deleteModel(const QString &filePath);

signals:
    void modelsChanged();

private:
    ModelInfo extractInfo(const QString &path) const;
};
