#!/bin/bash
# Build and install llama.cpp with optimal GPU configuration
set -e

echo "============================================"
echo "  Setting up llama.cpp"
echo "============================================"

LLAMA_DIR="$HOME/.local/share/lingmo-ai-assistant/llama.cpp"
MODELS_DIR="$HOME/.local/share/lingmo-ai-assistant/models"
BIN_DIR="$HOME/.local/bin"

mkdir -p "$MODELS_DIR" "$BIN_DIR"

# --- Detect GPU ---
HAS_CUDA=0; HAS_HIP=0; HAS_VULKAN=0

if command -v nvidia-smi &>/dev/null && nvidia-smi &>/dev/null 2>&1; then
    echo "[DETECT] NVIDIA GPU found"
    HAS_CUDA=1
elif command -v rocm-smi &>/dev/null && rocm-smi &>/dev/null 2>&1; then
    echo "[DETECT] AMD GPU found (ROCm)"
    HAS_HIP=1
elif command -v vulkaninfo &>/dev/null && vulkaninfo &>/dev/null 2>&1; then
    echo "[DETECT] Vulkan support found"
    HAS_VULKAN=1
else
    echo "[DETECT] CPU-only mode"
fi

# --- Clone / Update llama.cpp ---
if [ -d "$LLAMA_DIR" ]; then
    echo "[INFO] Updating existing llama.cpp..."
    cd "$LLAMA_DIR"
    git pull --ff-only 2>/dev/null || echo "[INFO] Using existing checkout"
else
    echo "[INFO] Cloning llama.cpp..."
    git clone --depth 1 https://github.com/ggerganov/llama.cpp.git "$LLAMA_DIR"
    cd "$LLAMA_DIR"
fi

# --- CMake configuration ---
echo "[INFO] Configuring build..."

CMAKE_ARGS="-B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF"

if [ $HAS_CUDA -eq 1 ]; then
    echo "[INFO] Enabling CUDA backend"
    CMAKE_ARGS="$CMAKE_ARGS -DGGML_CUDA=ON"
elif [ $HAS_HIP -eq 1 ]; then
    echo "[INFO] Enabling HIP (ROCm) backend"
    CMAKE_ARGS="$CMAKE_ARGS -DGGML_HIP=ON"
elif [ $HAS_VULKAN -eq 1 ]; then
    echo "[INFO] Enabling Vulkan backend"
    CMAKE_ARGS="$CMAKE_ARGS -DGGML_VULKAN=ON"
fi

cmake $CMAKE_ARGS

# --- Build ---
CORES=$(nproc)
echo "[INFO] Building with $CORES threads..."
cmake --build build --config Release -j$CORES

# --- Install binaries ---
echo "[INFO] Installing binaries..."
cp build/bin/llama-server "$BIN_DIR/"
cp build/bin/llama-cli "$BIN_DIR/" 2>/dev/null || true
cp build/bin/llama-bench "$BIN_DIR/" 2>/dev/null || true

# --- Add to PATH ---
if ! grep -q "$BIN_DIR" "$HOME/.bashrc" 2>/dev/null; then
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"
    echo "[INFO] Added $BIN_DIR to PATH in ~/.bashrc"
fi

export PATH="$BIN_DIR:$PATH"

echo ""
echo "============================================"
echo "  llama.cpp installed successfully!"
echo "  Binary: $(which llama-server)"
echo "  Models directory: $MODELS_DIR"
echo ""
echo "  Quick test:"
echo "    llama-server --help"
echo ""
echo "  Download a model:"
echo "    wget <model-url> -P $MODELS_DIR"
echo "============================================"
