# LingmoOS AI Assistant

Multi-backend AI chat client with Material Design 3 UI built on Qt6.

## Features

| Feature | Description |
|---------|-------------|
| **Multi-backend** | DeepSeek, Ollama (Local & Cloud), llama.cpp (GGUF) |
| **Streaming** | Real-time SSE / JSON-lines token streaming |
| **Context management** | Token-budget-based trimming, 8192 default history limit |
| **Hardware-aware** | Auto-detect CPU/GPU/VRAM for optimal local model config |
| **Auto-update** | GitHub release check with daily/weekly/monthly interval |
| **Custom fonts** | Outfit (UI), Noto Sans SC (CJK), JetBrains Mono (code) |
| **Material Design 3** | Light/dark theming with M3 color system |
| **Debian packaging** | Native .deb package for Debian Bookworm+ amd64 |

## Architecture

```
src/
├── core/           ChatController, ContextManager, TokenEstimator
├── providers/      AIProvider → DeepSeek, Ollama, LlamaCppProvider
├── network/        NetworkClient, StreamParser (SSE/JSON-lines)
├── models/         HardwareDetector, ModelManager (GGUF), ModelDownloader
└── utils/          Logger, Platform
resources/qml/      Qt6 QML UI (ChatView, Sidebar, Settings, SetupWizard)
scripts/            Shell install/setup helpers
debian/             dpkg-buildpackage configuration
```

## Prerequisites

- Debian 12 (Bookworm) or later
- Qt 6.4+ development packages
- CMake 3.22+, Ninja (optional)
- GCC 12+ or Clang 15+

## Quick Install

### From .deb

```bash
sudo apt install ./lingmo-assistant_0.1.0-1_amd64.deb
sudo apt install -f
lingmo-assistant
```

### Build from source

```bash
git clone https://github.com/arkimium/lingmo-assistant.git
cd lingmo-assistant
```

#### Install dependencies

```bash
# One-liner
make deps

# Or manually:
sudo apt install -y \
    build-essential cmake ninja-build pkg-config \
    qt6-base-dev qt6-declarative-dev qt6-tools-dev \
    qt6-tools-dev-tools qt6-svg-dev qt6-base-dev-tools \
    libqt6network6 libqt6core6 libqt6qml6 libqt6quick6 \
    qml6-module-qtquick-controls qml6-module-qtquick-layouts \
    qml6-module-qtquick-templates qml6-module-qtquick-dialogs \
    qml6-module-qtquick-window qml6-module-qtqml \
    qml6-module-qtqml-workerscript qt6-qpa-plugins \
    libgl1-mesa-dev libvulkan-dev libcurl4-openssl-dev \
    libssl-dev libfontconfig1-dev libfreetype6-dev
```

#### Build and run

```bash
make build
make run

# Or build .deb package
make deb-docker
```

### Makefile targets

| Command | Description |
|---------|-------------|
| `make build` | Build with CMake (Release) |
| `make run` | Build and launch |
| `make deb` | Build .deb natively |
| `make deb-docker` | Build .deb in Docker container |
| `make deps` | Install system dependencies via apt |
| `make test` | Quick smoke test |
| `make test-docker` | Full test with Xvfb in Docker |
| `make install` | Install to /usr/local/bin |
| `make clean` | Clean build artifacts |
| `make lint` | Lintian quality check |

### Manual build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/lingmo-assistant
```

## Configuration

### First run

The app auto-runs an initialization wizard on first launch:
- Checks system environment
- Downloads fonts (Outfit, Noto Sans SC, JetBrains Mono)
- Detects Ollama and llama.cpp availability
- Configures API keys

Alternatively, run the terminal wizard:

```bash
bash scripts/first-run-wizard.sh
```

### Provider setup

| Provider | Type | Setup |
|----------|------|-------|
| DeepSeek | Cloud | Enter API key in Settings |
| Ollama Local | Local | `bash scripts/setup-ollama.sh` |
| Ollama Cloud | Cloud | Configure endpoint in Settings |
| llama.cpp | Local | `bash scripts/setup-llama-cpp.sh` |

### Context window

Default history limit: **8192 tokens**. Adjust in Settings → Context.
Uses adaptive Chinese/English token estimation with 85% safety margin.

## License

GNU General Public License v3.0
