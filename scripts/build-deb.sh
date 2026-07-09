#!/bin/bash
# Build lingmo-assistant .deb package in Docker
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
IMAGE_NAME="lingmo-builder:latest"
PACKAGE_NAME="lingmo-assistant"

echo "============================================"
echo "  Building $PACKAGE_NAME .deb in Docker"
echo "============================================"
echo ""

# Step 1: Build Docker image
echo "[1/3] Building Docker image: $IMAGE_NAME"
cd "$PROJECT_DIR"
docker build -f debian/Dockerfile -t "$IMAGE_NAME" .

echo ""
echo "[2/3] Running dpkg-buildpackage in container..."
docker run --rm \
    -v "$PROJECT_DIR":/build \
    "$IMAGE_NAME" \
    bash -c "
        export DEBIAN_FRONTEND=noninteractive
        dpkg-buildpackage -us -uc -b 2>&1
    " || true

echo ""
echo "[3/3] Checking artifacts..."
ARTIFACTS_DIR="$PROJECT_DIR/.."
echo "  Looking for .deb in: $ARTIFACTS_DIR"
ls -la "$ARTIFACTS_DIR"/*.deb 2>/dev/null && echo ""
echo "============================================"
echo "  Build complete!"
if ls "$ARTIFACTS_DIR"/lingmo-assistant_*.deb 1>/dev/null 2>&1; then
    echo "  Package: $(ls -la "$ARTIFACTS_DIR"/lingmo-assistant_*.deb | head -1)"
else
    echo "  WARNING: No .deb found. Check build output above."
fi
echo "============================================"
