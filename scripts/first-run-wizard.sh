#!/bin/bash
# Lingmo AI Assistant ¡ª First Run Setup Wizard
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

echo ""
echo "  ¨X¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨["
echo "  ¨U     Lingmo AI Assistant ¡ª First Run     ¨U"
echo "  ¨^¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨a"
echo ""

# ============================================
# Step 1: System dependencies
# ============================================
echo "©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´"
echo "©¦ [1/6] System Dependencies               ©¦"
echo "©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼"

MISSING=""
for pkg in cmake ninja-build qt6-base-dev; do
    dpkg -s "$pkg" >/dev/null 2>&1 || MISSING="$MISSING $pkg"
done

if [ -n "$MISSING" ]; then
    echo "  Missing packages:$MISSING"
    read -p "  Install now? [Y/n] " yn
    if [ "$yn" != "n" ] && [ "$yn" != "N" ]; then
        bash "$SCRIPT_DIR/install-deps.sh"
    fi
else
    echo "  [OK] All system dependencies satisfied"
fi

# ============================================
# Step 2: API Keys
# ============================================
echo ""
echo "©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´"
echo "©¦ [2/6] API Keys (press Enter to skip)    ©¦"
echo "©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼"

read -p "  DeepSeek API Key: " DEEPSEEK_KEY
if [ -n "$DEEPSEEK_KEY" ]; then
    echo "  [OK] DeepSeek key saved"
fi

# ============================================
# Step 3: llama.cpp
# ============================================
echo ""
echo "©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´"
echo "©¦ [3/6] Local Backend: llama.cpp          ©¦"
echo "©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼"

read -p "  Install llama.cpp for local GGUF models? [Y/n] " yn
if [ "$yn" != "n" ] && [ "$yn" != "N" ]; then
    bash "$SCRIPT_DIR/setup-llama-cpp.sh"
fi

# ============================================
# Step 4: Ollama
# ============================================
echo ""
echo "©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´"
echo "©¦ [4/6] Local Backend: Ollama             ©¦"
echo "©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼"

read -p "  Install Ollama for local models? [Y/n] " yn
if [ "$yn" != "n" ] && [ "$yn" != "N" ]; then
    bash "$SCRIPT_DIR/setup-ollama.sh"
fi

# ============================================
# Step 5: Fonts
# ============================================
echo ""
echo "©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´"
echo "©¦ [5/6] Custom Fonts                      ©¦"
echo "©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼"

read -p "  Download custom fonts (Outfit, Noto Sans SC, JetBrains Mono)? [Y/n] " yn
if [ "$yn" != "n" ] && [ "$yn" != "N" ]; then
    bash "$SCRIPT_DIR/download-fonts.sh"
fi

# ============================================
# Step 6: Build
# ============================================
echo ""
echo "©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´"
echo "©¦ [6/6] Build Application                 ©¦"
echo "©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼"

read -p "  Build Lingmo AI Assistant now? [Y/n] " yn
if [ "$yn" != "n" ] && [ "$yn" != "N" ]; then
    cd "$PROJECT_DIR"
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release -j$(nproc)
    echo ""
    echo "  [OK] Build complete!"
    echo "  Binary: $PROJECT_DIR/build/lingmo-ai-assistant"
fi

# ============================================
# Done
# ============================================
echo ""
echo "  ¨X¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨["
echo "  ¨U           Setup Complete!               ¨U"
echo "  ¨U                                        ¨U"
echo "  ¨U  Run: ./build/lingmo-ai-assistant      ¨U"
echo "  ¨^¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨a"
echo ""
