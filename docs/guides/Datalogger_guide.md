# Vehicle Data Logger — "Black Box"

## 1. Concept

The data logger acts as the vehicle's **black box**: a background service that continuously records every signal flowing through the system — sensor readings, actuator commands, ADAS events, heartbeats, and hardware health — into structured session files.

Every time the car boots, a new session file is created. When the car shuts down (or the service is stopped), the session is closed. The result is a self-contained, timestamped record of everything that happened during that run, ready for offline analysis.

### Why this matters

The vehicle already has two real-time interfaces: the Qt6 instrument cluster and the touchscreen HMI, both rendering live VSS data on-screen, inside the car. These serve the driver — they show what the car is doing right now. The data logger adds a different dimension: a way to observe and review the vehicle's state from **outside the car**, on any device with a browser — a laptop in the pit area, a smartphone trackside, or a workstation in another room. This gives the team visibility into the car's behavior without needing to be physically in it.

Beyond live monitoring, the core value is post-mortem analysis. On a small autonomous platform with limited real-time debugging capability, most problems are diagnosed after the fact. A motor stalled, AEB triggered unexpectedly, the STM32 heartbeat dropped — without recorded data, the team is left guessing. The data logger eliminates this by capturing the full timeline of the vehicle's state, making it possible to:

- Reconstruct what happened before and during an incident
- Compare driving sessions across different software versions or track conditions
- Validate ADAS behavior (AEB reaction times, lane detection accuracy) with real data
- Monitor hardware health trends (temperatures, battery voltage, CAN bus errors)
- Replay recorded sessions through the databroker for HMI development without the physical car

## 2. Architecture Decisions

### Why Kuksa.val / VSS

The system already runs a COVESA VSS databroker (Kuksa.val) as the central vehicle data bus. All sensor data from the STM32 arrives via CAN, gets translated into VSS paths by the CAN-VSS bridge, and is published to the databroker over gRPC. This means the data logger doesn't need to instrument anything new — it simply subscribes to what is already there.

Using VSS as the data vocabulary also gives us a **standardized signal namespace**. Instead of logging raw CAN IDs (0x180, 0x280) that require a separate DBC file to decode, every signal is already human-readable (`Vehicle.Speed`, `Vehicle.ADAS.AEB.IsActive`, `Vehicle.Powertrain.Battery.Voltage`). Any engineer can open a log file and immediately understand what each value represents.

### Why JSONL (JSON Lines)

Each log file uses the JSON Lines format — one JSON object per line, no wrapping array, no commas between records. This was chosen over CSV and binary formats for several reasons:

- **Append-safe**: the service writes one line at a time. If the car loses power mid-session, all previously written lines remain valid. A CSV with a corrupted header or a binary file with an incomplete footer would be harder to recover.
- **Schema-flexible**: different record types (telemetry, system metrics, events) coexist in the same file without needing separate schemas or tables. Each line carries its own `type` field.
- **Grep-friendly**: filtering for a specific signal or event type is a one-liner with standard Unix tools (`grep`, `jq`).
- **Streaming-compatible**: the offline viewer (and any future web viewer) can process the file line by line without loading everything into memory.

### Why a systemd service

The data logger runs as a systemd unit (`seame-datalogger.service`) that starts automatically on boot, ordered after `kuksa-val.service` and `can-vss-bridge.service` to guarantee the databroker is available before subscriptions begin. This ensures logging starts without manual intervention and enables clean shutdown handling — when the service receives `SIGTERM`, it writes a `session_end` record and flushes the file before exiting.

The service is sandboxed with resource limits (128 MB RAM cap, 15% CPU quota, `ProtectSystem=strict`) to prevent the logger from competing with safety-critical processes like ADAS inference.

### Why `/data/logs/`

The `/data/` partition survives RAUC A/B OTA updates. Logs written here persist across system reflashes, which means recorded sessions are never lost when the team deploys a new AGL image. The config file also lives in `/data/seame-configs/` for the same reason.

V1 - DataLogger.html 
<img width="1570" height="918" alt="Captura de ecrã de 2026-03-28 01-19-05" src="https://github.com/user-attachments/assets/3f2b4651-a2e1-4065-90d2-6638c5b89de0" />
<img width="1570" height="918" alt="Captura de ecrã de 2026-03-28 01-19-19" src="https://github.com/user-attachments/assets/b89418b3-5eb4-459b-8c43-3d0a4745d887" />


## 3. Data Flow

```
STM32 (ThreadX)
    │
    │ CAN bus (500 kbps)
    ▼
CAN-VSS Bridge (Python)
    │
    │ gRPC publish
    ▼
Kuksa.val Databroker (port 55555, TLS)
    │
    │ gRPC subscribe          sysfs / procfs
    ▼                              │
┌──────────────────────────────────┤
│       seame-datalogger.py        │
│  ┌────────────┐  ┌────────────┐  │
│  │ VSS Thread  │  │ Sysfs Poll │  │
│  │ (telemetry) │  │ (system)   │  │
│  └──────┬──────┘  └──────┬─────┘  │
│         │                │        │
│         ▼                ▼        │
│     SessionWriter (JSONL)         │
│         │                         │
└─────────┼─────────────────────────┘
          ▼
  /data/logs/session_YYYYMMDD_HHMMSS.jsonl
          │
          ├──► NGINX (port 80) ──► Web Viewer (live mode)
          │
          └──► scp / drag-and-drop ──► Web Viewer (offline mode)
```

## 4. Log Format

### Session file naming

```
/data/logs/session_20260323_143200.jsonl
                   ────────────────
                   date     time (UTC)
```

### Record types

Every line is a JSON object with at least `ts` (ISO 8601 timestamp) and `type`:

| Type | When written | Content |
|------|-------------|---------|
| `session_start` | Once at boot | Session ID, hostname, uptime, AGL version, kernel, RAUC slot, loaded HEF models, git hash |
| `telemetry` | Per subscription frequency (1–50 Hz) | VSS datapoints from Kuksa.val (speed, steering, IMU, battery, sensors) |
| `system` | 1 Hz | RPi5 CPU temp, Hailo temp (via `hailortcli` or hwmon fallback), CPU/RAM usage, disk, CAN bus errors, WiFi RSSI |
| `event` | On occurrence | AEB triggers, heartbeat timeouts (with `elapsed_s` and `last_counter`), LDW alerts, errors |
| `session_end` | On graceful shutdown | Shutdown timestamp, reason, session duration |

### Example records

```json
{"ts":"2026-03-23T14:32:00.000Z","type":"session_start","data":{"session_id":"0379f788","hostname":"seame-agl","kernel":"6.6.63-v8-16k","agl_version":"Automotive Grade Linux 20.0.2 (trout)","uptime_s":15.88,"rauc_slot":"A","hef_models":["yolov8n.hef"]}}
{"ts":"2026-03-23T14:32:00.123Z","type":"telemetry","data":{"Vehicle.Speed":12.5,"Vehicle.Powertrain.Motor.Speed":1500,"Vehicle.Chassis.SteeringWheel.Angle":-3.2}}
{"ts":"2026-03-23T14:32:00.123Z","type":"system","data":{"cpu_temp_c":58.2,"hailo_temp_c":45.0,"cpu_percent":23.1,"ram_used_mb":412,"can0_state":"ERROR-ACTIVE"}}
{"ts":"2026-03-23T14:32:05.800Z","type":"event","data":{"event":"aeb_triggered","speed":8.2,"distance_m":0.35}}
{"ts":"2026-03-23T14:32:08.200Z","type":"event","data":{"event":"stm32_heartbeat_timeout","elapsed_s":3.5,"last_counter":142}}
{"ts":"2026-03-23T14:45:00.000Z","type":"session_end","data":{"reason":"SIGTERM","duration_s":780}}
```

## 5. Configuration

The service reads its config from `/data/seame-configs/datalogger/datalogger_config.yaml`, falling back to `/opt/seame/datalogger/datalogger_config.yaml` if absent. Key sections:

| Section | Purpose |
|---------|---------|
| `kuksa` | Databroker host, port, TLS certificate and JWT token paths (`/etc/kuksa/tls/`) |
| `logging` | Output directory, max file size (50 MB), max files (10), max total size (500 MB), in-process rotation by SessionWriter |
| `vss_subscriptions` | Groups of VSS paths with per-group sample rates (e.g., speed at 10 Hz, battery at 1 Hz). Event-driven signals (AEB, LDW) use `frequency: 0` — logged immediately on change, not polled |
| `sysfs_polls` | Local system metrics read from sysfs/procfs (CPU temp, RAM, disk, CAN errors, WiFi RSSI) |
| `heartbeat` | STM32 heartbeat timeout detection (timeout triggers an event record with `elapsed_s` and `last_counter`), RPi5 heartbeat publish rate back to Kuksa |

Separating fast signals (IMU at 50 Hz) from slow signals (battery at 1 Hz) keeps log file sizes manageable while preserving the temporal resolution needed for each type of data.

### Signal coverage

The system logs approximately 55 signals, organized into the following categories:

| Category | Examples |
|----------|----------|
| Temperatures | RPi5 CPU, Hailo-8, STM32, ambient (HTS221) |
| Environment | Humidity, barometric pressure, ambient light |
| IMU | Accelerometer (3-axis), gyroscope (3-axis) |
| Magnetometer | Heading (3-axis, IIS2MDC) |
| Distance | SRF08 ultrasonic (AEB), VL53L5CX ToF |
| Power | Battery voltage/current/power (INA226), state of charge |
| Actuators | Motor speed/direction, servo steering angle |
| ADAS | Inference FPS, AEB active/triggered, LDW alerts, detected object count |
| System health | CPU/RAM/disk usage, CAN bus errors, WiFi RSSI |
| Heartbeats | STM32 and RPi5 heartbeat counters and timeouts |

The complete mapping between CAN frame IDs, VSS paths, units, and sample rates is maintained in the signal registry spreadsheet (`seame_datalogger_signal_registry.xlsx`).

<img width="996" height="729" alt="seame_datalogger_signal_registry" src="https://github.com/user-attachments/assets/2fb28d1c-7855-4236-b785-bb636355075f" />

## 6. File Layout

```
/data/
├── logs/                                  # Session log files
│   ├── session_20260323_143200.jsonl
│   ├── latest.jsonl                       # Symlink → current session (used by live viewer)
│   └── ...
└── seame-configs/
    └── datalogger/
        └── datalogger_config.yaml         # Persistent config

/opt/seame/datalogger/
├── seame_datalogger.py                    # Service script
├── datalogger_config.yaml                 # Default config
├── seame-datalogger.logrotate             # Logrotate config (supplementary to in-process rotation)
├── seame_datalogger_signal_registry.xlsx   # CAN ID → VSS path mapping for all ~55 signals
├── docker-compose.yaml                    # NGINX + viewer deployment
└── json_viewer/                           # Web viewer (HTML + Chart.js)

/etc/systemd/system/
└── seame-datalogger.service               # Systemd unit
```

## 7. Analysis & Visualization

Session files can be analyzed in three ways:

**Command line** — JSONL is grep/jq-friendly. Filtering by record type, extracting a single signal over time, or exporting to CSV for spreadsheet analysis are all one-liners.

**Offline viewer** — A standalone HTML file (no server required) that loads a `.jsonl` session file via drag-and-drop, parses it client-side, and renders interactive Chart.js time-series plots. The viewer can overlay multiple signals, zoom into specific time windows, and highlight events (AEB triggers, heartbeat drops) on the timeline. This file can be opened on any laptop with a browser — no dependencies, no install.

**Live web dashboard** — An NGINX instance (deployed via Docker Compose on port 80) serves the `/data/logs/` directory with CORS headers. The datalogger service maintains a `latest.jsonl` symlink pointing to the current session file. The same HTML viewer, when accessed at `http://<car-ip>/`, polls `latest.jsonl` in live mode, displaying real-time telemetry and events as the car drives — no `scp` needed, any browser on the same network can connect.

```
Live mode:    Browser ──► http://10.21.220.191/ ──► NGINX ──► /data/logs/latest.jsonl
Offline mode: Browser ──► drag-and-drop local .jsonl file
```

Because the viewer is a pure client-side HTML application and the backend is a standard NGINX file server, the same setup scales beyond the local network with minimal effort. Exposing the car's NGINX through a reverse proxy, a VPN, or simply hosting the session files on any public web server would make the dashboard accessible from anywhere — the viewer doesn't care whether the `.jsonl` file comes from `10.21.220.x` or a public URL.

## 8. Deployment Notes

A few issues were encountered and resolved during the initial deployment that are worth documenting for future reference:

**Stale config surviving reflash** — Because `/data/` persists across RAUC updates, an outdated `datalogger_config.yaml` from a previous version survived a system reflash and caused the service to subscribe to VSS paths that no longer existed. The fix was to replace the config in `/data/seame-configs/` with the updated version after the reflash. This is a general consideration for any config stored on the persistent partition.

**VSSClient context manager** — The `kuksa-client` 0.4.x Python library requires `VSSClient` to be used as a context manager (`with VSSClient(...) as client:`), not via a `.connect()` method. Since the datalogger is a long-running service, this required structuring the connection lifecycle inside a dedicated thread that manages the context manager and reconnects on failure.

**TLS certificate handling** — The Kuksa databroker uses TLS with certificates stored at `/etc/kuksa/tls/`. The `VSSClient` expects the CA certificate path as a `Path` object, not raw bytes. The JWT token file must also be read and passed as a string. Both are configured in the `kuksa` section of the YAML config.
