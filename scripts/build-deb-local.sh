#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
IMAGE="lingmo-builder:latest"
OUTPUT_DIR="$PROJECT_DIR/../"

echo "=== Build lingmo-assistant .deb ==="
cd "$PROJECT_DIR"

# Build Docker image if needed
if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
    echo "[1/3] Building Docker image..."
    docker build -f debian/Dockerfile -t "$IMAGE" .
else
    echo "[1/3] Docker image exists: $IMAGE"
fi

# Build .deb
echo "[2/3] Building .deb package..."
docker run --rm \
    -v "$PROJECT_DIR:/workspace" \
    -v "$OUTPUT_DIR:/output" \
    "$IMAGE" \
    bash -c "
        cd /workspace
        chmod -x debian/*.install debian/*.docs debian/*.links 2>/dev/null || true
        dpkg-buildpackage -us -uc -b -d 2>&1
        cp /*.deb /*.changes /*.buildinfo /output/ 2>/dev/null || true
    "

# Verify
echo "[3/3] Artifacts:"
ls -lh "$OUTPUT_DIR"/lingmo-assistant_* 2>/dev/null || echo "  No artifacts found!"
echo "=== Done ==="
