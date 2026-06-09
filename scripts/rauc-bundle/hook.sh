#!/bin/bash
case "$1" in
  slot-install)
    VERSION="${RAUC_MF_VERSION:-unknown}"
    NEW_DIR="/data/apps/$VERSION"
    mkdir -p "$NEW_DIR"
    tar -xzf "$RAUC_BUNDLE_MOUNT_POINT/apps.tar.gz" -C "$NEW_DIR"
    echo "[RAUC] Extracted $VERSION to $NEW_DIR"
    ;;

  slot-post-install)
    VERSION="${RAUC_MF_VERSION:-unknown}"
    PREV=$(readlink /data/current)
    NEW_DIR="/data/apps/$VERSION"

    ln -sfn "$NEW_DIR" /data/current

    systemctl stop adas-manager inference 2>/dev/null || true
    systemctl start adas-manager
    sleep 10

    if ! systemctl is-active --quiet adas-manager; then
        echo "[RAUC] adas-manager failed — rolling back to $PREV"
        ln -sfn "$PREV" /data/current
        systemctl restart adas-manager 2>/dev/null || true
        exit 1
    fi

    systemctl start inference
    echo "[RAUC] Update OK — running $VERSION"

    # Keep only last 2 versions
    ls -dt /data/apps/v* 2>/dev/null | tail -n +3 | xargs rm -rf 2>/dev/null || true
    ;;
esac
