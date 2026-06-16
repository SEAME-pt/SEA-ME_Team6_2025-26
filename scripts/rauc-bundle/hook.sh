#!/bin/bash

CLUSTER_IP="10.21.220.192"
CLUSTER_SSH_KEY="/root/.ssh/id_cluster"
CLUSTER_APP_PATH="/home/ClusterApp"
CLUSTER_SERVICE="helloqt-app.service"

get_version() {
    local v="${RAUC_MF_VERSION}"
    if [ -z "$v" ] && [ -f "$RAUC_BUNDLE_MOUNT_POINT/manifest.raucm" ]; then
        v=$(grep "^version=" "$RAUC_BUNDLE_MOUNT_POINT/manifest.raucm" | cut -d= -f2)
    fi
    echo "${v:-unknown}"
}

case "$1" in
  slot-install|install)
    VERSION=$(get_version)
    PREV=$(readlink /data/current)
    NEW_DIR="/data/apps/$VERSION"

    mkdir -p "$NEW_DIR"
    tar -xzf "$RAUC_BUNDLE_MOUNT_POINT/apps.tar.gz" -C "$NEW_DIR"
    echo "[RAUC] Extracted $VERSION to $NEW_DIR"

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
    echo "[RAUC] RPi5 update OK — running $VERSION"

    # ── Deploy ClusterApp to RPi4 if included in bundle ──────────────────────
    CLUSTER_BIN="$NEW_DIR/cluster/ClusterApp"
    if [ -f "$CLUSTER_BIN" ]; then
        echo "[RAUC] Deploying ClusterApp to RPi4 ($CLUSTER_IP)..."

        # Stop service + backup before overwriting
        ssh -i "$CLUSTER_SSH_KEY" -o StrictHostKeyChecking=no \
            "root@${CLUSTER_IP}" \
            "systemctl stop ${CLUSTER_SERVICE}; [ -f ${CLUSTER_APP_PATH} ] && cp ${CLUSTER_APP_PATH} ${CLUSTER_APP_PATH}.bak || true"

        if scp -i "$CLUSTER_SSH_KEY" -o StrictHostKeyChecking=no \
               "$CLUSTER_BIN" "root@${CLUSTER_IP}:${CLUSTER_APP_PATH}"; then

            ssh -i "$CLUSTER_SSH_KEY" -o StrictHostKeyChecking=no \
                "root@${CLUSTER_IP}" "systemctl restart ${CLUSTER_SERVICE}"
            sleep 5

            # Watchdog — check cluster is running
            if ssh -i "$CLUSTER_SSH_KEY" -o StrictHostKeyChecking=no \
                   "root@${CLUSTER_IP}" \
                   "systemctl is-active --quiet ${CLUSTER_SERVICE}"; then
                echo "[RAUC] Cluster updated OK"
            else
                echo "[RAUC] Cluster failed — rolling back ClusterApp"
                ssh -i "$CLUSTER_SSH_KEY" -o StrictHostKeyChecking=no \
                    "root@${CLUSTER_IP}" \
                    "[ -f ${CLUSTER_APP_PATH}.bak ] && \
                     mv ${CLUSTER_APP_PATH}.bak ${CLUSTER_APP_PATH} && \
                     systemctl restart ${CLUSTER_SERVICE} || true"
            fi
        else
            echo "[RAUC] Cluster scp failed — keeping previous version"
        fi
    else
        echo "[RAUC] No ClusterApp in bundle — skipping cluster update"
    fi

    # ── Flash STM32 firmware if included in bundle ────────────────────────────
    STM32_BIN="$NEW_DIR/team6_original.bin"
    STM32_BACKUP="/data/stm32/firmware.bak.bin"
    STM32_IF="/usr/share/openocd/scripts/interface/stlink.cfg"
    STM32_TGT="/usr/share/openocd/scripts/target/stm32u5x.cfg"

    stm32_flash() {
        openocd -f "$STM32_IF" -f "$STM32_TGT" \
            -c "program $1 0x08000000 verify reset exit"
    }

    stm32_alive() {
        openocd -f "$STM32_IF" -f "$STM32_TGT" \
            -c "init; reset run; sleep 2000; halt; exit" 2>&1 \
            | grep -q "halted due to debug-request"
    }

    if [ -f "$STM32_BIN" ]; then
        echo "[RAUC] Flashing STM32 firmware..."
        mkdir -p /data/stm32

        if stm32_flash "$STM32_BIN"; then
            echo "[RAUC] STM32 flash OK — verifying..."
            sleep 3
            if stm32_alive; then
                echo "[RAUC] STM32 alive — update OK"
                cp "$STM32_BIN" "$STM32_BACKUP"
            else
                echo "[RAUC] STM32 not responding — rolling back"
                if [ -f "$STM32_BACKUP" ]; then
                    stm32_flash "$STM32_BACKUP" \
                        && echo "[RAUC] STM32 rollback OK" \
                        || echo "[RAUC] STM32 rollback failed"
                else
                    echo "[RAUC] No STM32 backup available"
                fi
            fi
        else
            echo "[RAUC] STM32 flash failed — keeping previous firmware"
        fi
    else
        echo "[RAUC] No STM32 firmware in bundle — skipping"
    fi

    ls -dt /data/apps/v* 2>/dev/null | tail -n +3 | xargs rm -rf 2>/dev/null || true
    ;;
esac
