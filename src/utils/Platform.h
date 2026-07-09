#pragma once
#include <QString>

namespace Platform {
    QString dataDir();
    QString configDir();
    QString cacheDir();
    QString modelsDir();
    QString fontsDir();
    bool isDebian();
}
