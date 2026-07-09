#!/bin/bash
# Download custom fonts from Google Fonts via ghproxy.vip mirror
set -e

echo "============================================"
echo "  Downloading Fonts"
echo "============================================"

FONTS_DIR="$HOME/.local/share/lingmo-ai-assistant/fonts"
mkdir -p "$FONTS_DIR"

# Google Fonts source (GitHub raw)
GH_RAW="https://raw.githubusercontent.com/google/fonts/main"
GH_PROXY="https://ghproxy.vip/raw.githubusercontent.com/google/fonts/main"

# Test which source is accessible
if wget -q --timeout=5 --spider "$GH_RAW/ofl/outfit/Outfit%5Bwght%5D.ttf" 2>/dev/null; then
    BASE_URL="$GH_RAW"
    echo "[INFO] Using direct GitHub source"
else
    BASE_URL="$GH_PROXY"
    echo "[INFO] Using ghproxy.vip mirror"
fi

# --- Font definitions ---
declare -A FONTS=(
    ["Outfit[wght].ttf"]="$BASE_URL/ofl/outfit/Outfit%5Bwght%5D.ttf"
    ["NotoSansSC[wght].ttf"]="$BASE_URL/ofl/notosanssc/NotoSansSC%5Bwght%5D.ttf"
    ["JetBrainsMono[wght].ttf"]="$BASE_URL/ofl/jetbrainsmono/JetBrainsMono%5Bwght%5D.ttf"
)

DOWNLOADED=0
SKIPPED=0

for filename in "${!FONTS[@]}"; do
    url="${FONTS[$filename]}"
    target="$FONTS_DIR/$filename"

    if [ -f "$target" ]; then
        size=$(stat -c%s "$target" 2>/dev/null || stat -f%z "$target" 2>/dev/null)
        if [ "$size" -gt 10000 ]; then
            echo "[SKIP] $filename (already downloaded)"
            SKIPPED=$((SKIPPED + 1))
            continue
        fi
    fi

    echo "[DOWNLOAD] $filename"
    wget -q --show-progress "$url" -O "$target" || {
        echo "[ERROR] Failed to download $filename"
        echo "        URL: $url"
    }
    DOWNLOADED=$((DOWNLOADED + 1))
done

echo ""
echo "============================================"
echo "  Fonts download complete!"
echo "  Downloaded: $DOWNLOADED  Skipped: $SKIPPED"
echo "  Directory:  $FONTS_DIR"
ls -lh "$FONTS_DIR" 2>/dev/null
echo "============================================"
