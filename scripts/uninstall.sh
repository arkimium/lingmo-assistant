#!/bin/bash
# Lingmo AI Assistant for LingmoOS Uninstall script
set -e

echo "============================================"
echo " Uninstalling Lingmo AI Assistant "
echo "============================================"
echo ""

# --- Remove binaries ---
echo "[INFO] Removing binaries..."
rm -f "$HOME/.local/bin/llama-server"
rm -f "$HOME/.local/bin/llama-cli"
rm -f "$HOME/.local/bin/llama-bench"

# --- Remove data ---
read -p "  Remove all data (conversations, models, fonts)? [y/N] " yn
if [ "$yn" = "y" ] || [ "$yn" = "Y" ]; then
    echo "[INFO] Removing data directory..."
    rm -rf "$HOME/.local/share/lingmo-ai-assistant"
    echo "[OK] Data removed"
else
    echo "[SKIP] Data preserved at ~/.local/share/lingmo-ai-assistant"
fi

# --- Remove config ---
read -p "  Remove configuration (API keys, settings)? [y/N] " yn
if [ "$yn" = "y" ] || [ "$yn" = "Y" ]; then
    echo "[INFO] Removing config..."
    rm -rf "$HOME/.config/Lingmo"
    echo "[OK] Config removed"
else
    echo "[SKIP] Config preserved"
fi

# --- Remove Ollama service ---
if systemctl --user is-enabled ollama >/dev/null 2>&1; then
    read -p "  Disable Ollama user service? [y/N] " yn
    if [ "$yn" = "y" ] || [ "$yn" = "Y" ]; then
        systemctl --user disable ollama
        systemctl --user stop ollama
        rm -f "$HOME/.config/systemd/user/ollama.service"
        echo "[OK] Ollama service removed"
    fi
fi

echo ""
echo "============================================"
echo "  Uninstall complete."
echo "============================================"
