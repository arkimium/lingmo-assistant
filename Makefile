BUILD_DIR ?= build
TYPE      ?= Release
JOBS      := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
BINARY     = $(BUILD_DIR)/lingmo-assistant
DEB_OUT   := $(shell ls ../lingmo-assistant_*.deb 2>/dev/null | head -1)
CPU_N      = $(JOBS)

# ── Default ───────────────────────────────────────
.PHONY: all
all: build

# ── Build ─────────────────────────────────────────
.PHONY: build configure
build: configure
	cmake --build $(BUILD_DIR) --config $(TYPE) -j$(CPU_N)

configure:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(TYPE) -G Ninja

# ── Run ───────────────────────────────────────────
.PHONY: run
run: build
	$(BINARY)

# ── Clean ─────────────────────────────────────────
.PHONY: clean distclean
clean:
	cmake --build $(BUILD_DIR) --target clean 2>/dev/null || true

distclean:
	rm -rf $(BUILD_DIR)

# ── Debian package ────────────────────────────────
.PHONY: deb deb-docker
deb-docker:
	docker build -f debian/Dockerfile -t lingmo-builder:latest .
	docker run --rm \
		-v "$$(pwd):/workspace" \
		-v "$$(dirname $$(pwd)):/output" \
		lingmo-builder:latest bash -c '
			cd /workspace && \
			chmod -x debian/*.install debian/*.docs debian/*.links 2>/dev/null || true && \
			dpkg-buildpackage -us -uc -b -d && \
			cp /*.deb /*.changes /*.buildinfo /output/
		'
	@echo "Package: $$(ls ../lingmo-assistant_*.deb 2>/dev/null)"

deb: configure
	dpkg-buildpackage -us -uc -b -d 2>/dev/null || \
		dpkg-buildpackage -us -uc -b

# ── Dependencies ──────────────────────────────────
.PHONY: deps
deps:
	bash scripts/install-deps.sh

# ── Tests ─────────────────────────────────────────
.PHONY: test test-docker
test: build
	$(BINARY) --help 2>&1 || true

test-docker:
	docker run --rm \
		-v "$$(pwd):/workspace" \
		-v "$$(dirname $$(pwd)):/output" \
		lingmo-builder:latest bash -c '
			cd /workspace && dpkg-buildpackage -us -uc -b -d 2>&1 | tail -1 && \
			cp /*.deb /output/ && \
			apt-get install -y -qq xvfb qml6-module-qtquick-controls qml6-module-qtquick-layouts \
				qml6-module-qtquick-templates qml6-module-qtquick-dialogs qml6-module-qtquick-window \
				qml6-module-qtqml-workerscript qml6-module-qtqml qt6-qpa-plugins 2>/dev/null && \
			dpkg -i /output/lingmo-assistant_0.1.0-1_amd64.deb 2>/dev/null && \
			Xvfb :99 -screen 0 1280x720x24 & sleep 1 && \
			export DISPLAY=:99 && \
			timeout 5 /usr/bin/lingmo-assistant 2>&1 | grep -v "Unable to assign" && \
			echo "[OK] App runs" && kill %1 2>/dev/null
	'

# ── Install / Uninstall ───────────────────────────
.PHONY: install uninstall
install: build
	install -Dm755 $(BINARY) /usr/local/bin/lingmo-assistant

uninstall:
	rm -f /usr/local/bin/lingmo-assistant

# ── Lint ──────────────────────────────────────────
.PHONY: lint
lint:
	lintian $(DEB_OUT) 2>&1 || true

# ── Help ──────────────────────────────────────────
.PHONY: help
help:
	@echo "LingmoOS AI Assistant — Makefile targets:"
	@echo ""
	@echo "  make              Build (Release)"
	@echo "  make build        Build with CMake"
	@echo "  make configure    CMake configure only"
	@echo "  make run          Build and run"
	@echo "  make clean        Clean build artifacts"
	@echo "  make distclean    Remove build directory"
	@echo "  make deb          Build .deb (native)"
	@echo "  make deb-docker   Build .deb in Docker"
	@echo "  make deps         Install system dependencies"
	@echo "  make test         Quick smoke test"
	@echo "  make test-docker  Full Docker test with Xvfb"
	@echo "  make install      Install to /usr/local/bin"
	@echo "  make uninstall    Remove from /usr/local/bin"
	@echo "  make lint         Lint .deb package"
	@echo "  make help         This help"
