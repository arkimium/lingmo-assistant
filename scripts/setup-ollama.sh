#!/bin/bash
# Install and configure Ollama
set -e

echo "============================================"
echo "  Setting up Ollama"
echo "============================================"

OLLAMA_INSTALLED=0

if command -v ollama &>/dev/null; then
    echo "[OK] Ollama already installed: $(which ollama)"
    OLLAMA_INSTALLED=1
fi

# --- Install ---
if [ $OLLAMA_INSTALLED -eq 0 ]; then
    echo "[INFO] Installing Ollama..."
    if curl -fsSL https://ollama.com/install.sh 2>/dev/null; then
        curl -fsSL https://ollama.com/install.sh | sh
    else
        echo "[WARN] Cannot reach ollama.com; trying manual install..."
        OLLAMA_VER="0.11.1"
        ARCH=$(uname -m)
        OLLAMA_URL="https://github.com/ollama/ollama/releases/download/v${OLLAMA_VER}/ollama-linux-${ARCH}.tgz"
        echo "[INFO] Downloading from GitHub..."
        cd /tmp
        wget -q --show-progress "$OLLAMA_URL" -O ollama.tgz
        tar xzf ollama.tgz
        sudo cp bin/ollama /usr/local/bin/
        rm -rf bin ollama.tgz
        echo "[OK] Ollama installed to /usr/local/bin/ollama"
    fi
    OLLAMA_INSTALLED=1
fi

# --- Start service ---
echo "[INFO] Ensuring Ollama is running..."
if curl -s http://localhost:11434/api/tags >/dev/null 2>&1; then
    echo "[OK] Ollama is already running on port 11434"
else
    echo "[INFO] Starting Ollama server..."
    nohup ollama serve > /tmp/ollama.log 2>&1 &
    sleep 3
    if curl -s http://localhost:11434/api/tags >/dev/null 2>&1; then
        echo "[OK] Ollama started successfully"
    else
        echo "[WARN] Ollama may not have started. Check /tmp/ollama.log"
    fi
fi

# --- Systemd user service ---
if command -v systemctl &>/dev/null; then
    mkdir -p "$HOME/.config/systemd/user"
    cat > "$HOME/.config/systemd/user/ollama.service" << 'SYSTEMD'
[Unit]
Description=Ollama AI Service
After=network.target

[Service]
ExecStart=/usr/local/bin/ollama serve
Restart=on-failure
Environment=OLLAMA_HOST=127.0.0.1

[Install]
WantedBy=default.target
SYSTEMD
    systemctl --user daemon-reload
    systemctl --user enable ollama 2>/dev/null || true
    echo "[INFO] systemd user service created"
fi

echo ""
echo "============================================"
echo "  Ollama setup complete!"
echo "  Endpoint: http://localhost:11434"
echo ""
echo "  Quick start:"
echo "    ollama pull qwen2.5:1.5b"
echo "    ollama run qwen2.5:1.5b"
echo "============================================"
