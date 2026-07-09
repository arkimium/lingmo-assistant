#include "TokenEstimator.h"
#include <cmath>

int CharTokenEstimator::estimate(const QString &text) const {
    int ascii = 0, cjk = 0;
    for (const QChar &c : text) {
        ushort u = c.unicode();
        if (u < 0x80) {
            ascii++;
        } else if ((u >= 0x4E00 && u <= 0x9FFF) ||
                   (u >= 0x3400 && u <= 0x4DBF) ||
                   (u >= 0xF900 && u <= 0xFAFF) ||
                   (u >= 0x3040 && u <= 0x309F) ||
                   (u >= 0x30A0 && u <= 0x30FF)) {
            cjk++;
        }
    }
    int other = text.length() - ascii - cjk;
    return static_cast<int>(std::ceil(ascii / 4.0 + cjk / 1.2 + other / 3.0));
}

int CharTokenEstimator::quickEstimate(const QString &text) const {
    return text.length() / 3;
}
