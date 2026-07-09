#!/bin/bash
# Lingmo AI Assistant for LingmoOS dependency installer
set -e

echo "============================================"
echo "  Building Lingmo AI Assistant Dependencies"
echo "============================================"
echo ""

# --- Detect Debian version ---
if [ -f /etc/debian_version ]; then
    DEB_VER=$(cat /etc/debian_version)
    echo "[INFO] Debian version: $DEB_VER"
else
    echo "[ERROR] This script requires Debian or Ubuntu"
    exit 1
fi

# --- Install core build tools ---
echo ""
echo "[1/5] Installing build essentials..."
sudo apt update
sudo apt install -y \
    build-essential cmake ninja-build pkg-config \
    git curl wget unzip ca-certificates

echo "[OK] Build tools installed"

# --- Install Qt6 ---
echo ""
echo "[2/5] Installing Qt6..."
sudo apt install -y \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    qt6-quickcontrols2-dev \
    qt6-svg-dev \
    qml6-module-qtquick-controls \
    qml6-module-qtquick-layouts \
    qml6-module-qtquick-window \
    qml6-module-qtqml-workerscript \
    qt6-base-dev-tools \
    libqt6network6 \
    libqt6core6 \
    libqt6qml6 \
    libqt6quick6 \
    libqt6quickcontrols2-6 \
    libqt6svg6 \
    libgl1-mesa-dev \
    libvulkan-dev \
    libcurl4-openssl-dev \
    libssl-dev \
    libfontconfig1-dev \
    libfreetype6-dev

echo "[OK] Qt6 installed"

# --- Detect GPU ---
echo ""
echo "[3/5] Detecting GPU..."
HAS_CUDA=0; HAS_HIP=0; HAS_VULKAN=0

if command -v nvidia-smi &>/dev/null && nvidia-smi &>/dev/null; then
    echo "[DETECT] NVIDIA GPU found"
    HAS_CUDA=1
    echo "[INFO] Installing CUDA support..."
    sudo apt install -y nvidia-cuda-toolkit 2>/dev/null || \
        echo "[WARN] CUDA toolkit not available from apt; install manually if needed"
elif command -v rocm-smi &>/dev/null && rocm-smi &>/dev/null; then
    echo "[DETECT] AMD GPU found"
    HAS_HIP=1
else
    echo "[DETECT] No GPU detected ¡ª CPU-only mode"
fi

if command -v vulkaninfo &>/dev/null && vulkaninfo &>/dev/null 2>&1; then
    echo "[DETECT] Vulkan support available"
    HAS_VULKAN=1
    sudo apt install -y libvulkan-dev vulkan-tools 2>/dev/null || true
fi

# --- Install GPU libraries ---
echo ""
echo "[4/5] Installing GPU libraries..."
if [ $HAS_CUDA -eq 1 ]; then
    sudo apt install -y nvidia-cuda-toolkit libcudnn8 2>/dev/null || true
elif [ $HAS_HIP -eq 1 ]; then
    sudo apt install -y rocm-libs rccl 2>/dev/null || true
fi

echo "[OK] GPU libraries ready"

# --- Verify ---
echo ""
echo "[5/5] Verifying installation..."
echo ""

echo -n "  cmake:     "; command -v cmake &>/dev/null && echo "$(cmake --version | head -1)" || echo "MISSING"
echo -n "  ninja:     "; command -v ninja &>/dev/null && echo "$(ninja --version)" || echo "MISSING"
echo -n "  g++:       "; command -v g++ &>/dev/null && echo "$(g++ --version | head -1)" || echo "MISSING"
echo -n "  Qt6 qmake: "; command -v qmake6 &>/dev/null && echo "$(qmake6 --version)" || echo "MISSING (try qmake6 or qmake)"
echo -n "  curl:      "; command -v curl &>/dev/null && echo "OK" || echo "MISSING"
echo -n "  wget:      "; command -v wget &>/dev/null && echo "OK" || echo "MISSING"
echo -n "  git:       "; command -v git &>/dev/null && echo "OK" || echo "MISSING"

echo ""
echo "============================================"
echo "  Dependencies installation complete!"
echo "  Next: run scripts/setup-llama-cpp.sh"
echo "        and scripts/setup-ollama.sh"
echo "============================================"
