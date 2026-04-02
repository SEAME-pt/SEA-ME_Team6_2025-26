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

## 4. How It Works — Step by Step

This section walks through what happens inside `seame_datalogger.py` from boot to shutdown, with the relevant code excerpts at each stage.

---

### Step 1 — Boot: config is loaded and components are created

When systemd starts the service, `main()` loads the YAML config and instantiates `DataLoggerService`, which creates all the subsystems in `__init__`:

```python
class DataLoggerService:
    def __init__(self, config: dict):
        self.writer      = SessionWriter(config)        # opens the first log file
        self.sys_collector = SystemCollector()          # no I/O yet — just init
        self.kuksa_sub   = KuksaSubscriber(config, self.writer)  # no connection yet
        self.hb_monitor  = HeartbeatMonitor(config, self.writer, self.kuksa_sub)
        self.ctrl_server = ControlServer(self.writer)   # HTTP API, not started yet
```

As soon as `SessionWriter` is created, it calls `_open_new_file()` internally, which creates the timestamped `.jsonl` file and updates the `latest.jsonl` symlink:

```python
def _open_new_file(self):
    ts = datetime.now().strftime("%Y%m%d_%H%M%S")
    self._file_path = self.output_dir / f"{self.prefix}_{ts}.jsonl"
    self._file = open(self._file_path, "a", encoding="utf-8")

    # Update latest.jsonl symlink so the web viewer always finds the live file
    latest = self.output_dir / "latest.jsonl"
    if latest.exists() or latest.is_symlink():
        latest.unlink()
    latest.symlink_to(self._file_path.name)
```

---

### Step 2 — `session_start` record is written

Before any sensor data is collected, `start()` gathers one-time metadata about the session — hostname, kernel version, AGL version, RAUC active slot, loaded AI models, git hash — and writes it as the first record in the file:

```python
def start(self):
    meta = collect_session_metadata()   # reads /etc/os-release, rauc status, /proc/uptime, ...
    meta["config_file"] = str(self._find_config_path())
    self.writer.write("session_start", meta)
```

`collect_session_metadata()` queries a few sources at boot:

```python
def collect_session_metadata() -> dict:
    meta = {
        "session_id": str(uuid.uuid4())[:8],   # short random ID, e.g. "0379f788"
        "hostname":   os.uname().nodename,
        "kernel":     os.uname().release,
    }
    # AGL version from /etc/os-release
    with open("/etc/os-release") as f:
        for line in f:
            if line.startswith("PRETTY_NAME="):
                meta["agl_version"] = line.split("=", 1)[1].strip().strip('"')
    # RAUC A/B slot
    out = subprocess.check_output(["rauc", "status", "--output-format=shell"]).decode()
    for line in out.splitlines():
        if "RAUC_SYSTEM_BOOTED_SLOT" in line:
            meta["rauc_active_slot"] = line.split("=", 1)[1].strip().strip('"')
    ...
    return meta
```

The result on disk looks like:
```json
{"ts":"2026-03-23T14:32:00.000Z","type":"session_start","data":{"session_id":"0379f788","hostname":"seame-agl","agl_version":"Automotive Grade Linux 20.0.2","rauc_active_slot":"A","uptime_s":15.88}}
```

---

### Step 3 — Threads start: VSS subscriptions and sysfs polling

After writing `session_start`, `start()` launches all background components:

```python
    self.ctrl_server.start()   # HTTP API thread (port 8080)
    self.kuksa_sub.start()     # one thread per VSS subscription group
    self.hb_monitor.start()    # heartbeat watchdog thread
    # then blocks in the sysfs poll loop (main thread)
```

**VSS subscription groups** are defined in the config, each with its own frequency:

```yaml
vss_subscriptions:
  motion:          { frequency_hz: 10,  paths: [Vehicle.Speed, Vehicle.Chassis.SteeringWheel.Angle, ...] }
  imu:             { frequency_hz: 10,  paths: [Vehicle.Acceleration.Lateral, ...] }
  power:           { frequency_hz: 2,   paths: [Vehicle.Powertrain.TractionBattery.CurrentVoltage, ...] }
  safety_events:   { frequency_hz: 0,   paths: [Vehicle.ADAS.AEB.IsActive, ...] }  # event-driven
```

For **polled** groups (`frequency_hz > 0`), `KuksaSubscriber` starts one thread per group that reads from Kuksa.val and writes a `telemetry` record at the configured rate:

```python
def _poll_loop(self, group_name, paths, freq_hz):
    interval = 1.0 / freq_hz
    while self._running:
        start = time.monotonic()
        data = {}
        for path in paths:
            entry = self._client.get_current_values([path])
            if entry and path in entry and entry[path].value is not None:
                data[path] = entry[path].value
        if data:
            self.writer.write("telemetry", data)
        time.sleep(max(0, interval - (time.monotonic() - start)))
```

For **event-driven** groups (`frequency_hz: 0`), a subscribe loop blocks until Kuksa pushes a change, then immediately logs it as an `event` record — no polling needed:

```python
def _subscribe_loop(self, group_name, paths):
    for updates in self._client.subscribe_current_values(paths):
        data = {path: val.value for path, val in updates.items() if val and val.value is not None}
        if data:
            self.writer.write("event", data)
```

---

### Step 4 — Sysfs polling runs in the main thread

While all VSS threads run in the background, the **main thread** loops at 1 Hz reading local system metrics directly from the Linux kernel — no Kuksa involved:

```python
while self._running:
    data = self.sys_collector.collect(sysfs_metrics)
    if data:
        self.writer.write("system", data)
    time.sleep(interval)
```

`SystemCollector` reads each metric from its source. For example, CPU temperature comes directly from the thermal subsystem in sysfs:

```python
@staticmethod
def _read_thermal(path: str) -> float:
    with open(path) as f:
        return int(f.read().strip()) / 1000.0
# path = "/sys/class/thermal/thermal_zone0/temp"  →  e.g. 58200 → 58.2 °C
```

CPU usage is derived by diffing two reads of `/proc/stat` (the same approach used by `top`):

```python
def _read_cpu_usage(self) -> float:
    with open("/proc/stat") as f:
        parts = f.readline().split()[1:]     # cpu  user nice system idle iowait ...
    vals = [int(x) for x in parts]
    idle, total = vals[3], sum(vals)
    d_idle  = idle  - self._prev_cpu[0]
    d_total = total - self._prev_cpu[1]
    self._prev_cpu = (idle, total)
    return round((1.0 - d_idle / d_total) * 100, 1)
```

---

### Step 5 — Every `write()` call serialises the record to disk

All threads (VSS pollers, event subscriber, main sysfs loop) call `writer.write()`. This method is thread-safe, handles automatic rotation when the file gets too large, and periodically calls `fsync` to make sure data survives a power cut:

```python
def write(self, record_type: str, data: dict):
    record = {
        "ts":   datetime.now(timezone.utc).isoformat(timespec="milliseconds"),
        "type": record_type,
        "data": data,
    }
    line = json.dumps(record, separators=(",", ":"), default=str) + "\n"

    with self._lock:                              # one writer at a time
        if self._bytes_written + len(line.encode()) > self.max_file_bytes:
            self._open_new_file()                 # auto-rotate at 50 MB

        self._file.write(line)
        self._bytes_written += len(line.encode())

        if time.monotonic() - self._last_flush >= self.flush_interval:
            self._file.flush()
            os.fsync(self._file.fileno())         # flush to storage every 5 s
            self._last_flush = time.monotonic()
```

The `threading.Lock` means that even if 10 VSS threads fire at the same millisecond, writes are serialised and the file stays valid JSONL with no interleaved or corrupted lines.

---

### Step 6 — Heartbeat watchdog runs independently

`HeartbeatMonitor` has its own thread that reads the STM32 heartbeat counter from Kuksa once per second. If the counter stops incrementing for longer than the configured timeout (default 3 s), it writes an `event` record — this is the signal that the STM32 has frozen or the CAN link has dropped:

```python
def _monitor_stm32(self, cfg):
    timeout = cfg.get("timeout_s", 3)
    while self._running:
        entry = self.kuksa._client.get_current_values([path])
        new_counter = entry[path].value
        if new_counter != self._stm32_counter:
            self._stm32_counter = new_counter
            self._stm32_last_seen = time.monotonic()   # heartbeat received → reset timer

        elapsed = time.monotonic() - self._stm32_last_seen
        if elapsed > timeout:
            self.writer.write("event", {
                "event":        "stm32_heartbeat_timeout",
                "elapsed_s":    round(elapsed, 1),
                "last_counter": self._stm32_counter,
            })
            self._stm32_last_seen = time.monotonic()   # reset to avoid log spam
        time.sleep(1.0)
```

A second thread in `HeartbeatMonitor` publishes the RPi5's own counter back to Kuksa at 2 Hz, so the STM32 can detect an RPi5 freeze by the same mechanism.

---

### Step 7 — The HTTP control API (ControlServer)

A small HTTP server runs in a daemon thread, exposing two endpoints for the web viewer:

| Endpoint | Method | What it does |
|----------|--------|-------------|
| `/api/status` | GET | Returns `{"current_file": "session_20260323_143200.jsonl"}` |
| `/api/rotate` | POST | Calls `writer.rotate()`, returns `{"new_file": "session_20260323_144500.jsonl"}` |

The "New Log" button in the web viewer calls `POST /api/rotate`, which triggers `_open_new_file()` via the thread-safe `rotate()` method — creating a new timestamped file and updating the `latest.jsonl` symlink atomically.

---

### Step 8 — Shutdown: `session_end` is written and the file is closed

When systemd sends `SIGTERM` (or Ctrl+C in development), the signal handler sets `_running = False`. The main loop exits, and `stop()` is called:

```python
def stop(self):
    self.ctrl_server.stop()
    self.hb_monitor.stop()
    self.kuksa_sub.stop()          # disconnects from Kuksa, joins VSS threads

    self.writer.write("session_end", {
        "end_time": datetime.now(timezone.utc).isoformat(),
        "reason":   "shutdown",
    })
    self.writer.close()            # final flush + fsync + file.close()
```

The `session_end` record closes the timeline cleanly. Any analysis of the log file can check for its presence — if it's missing, the session was terminated abruptly (power cut, kernel panic).

---

### Thread map

To summarise, at runtime the process runs these concurrent threads:

| Thread | What it does | Frequency |
|--------|-------------|-----------|
| Main thread | sysfs/procfs → `system` records | 1 Hz |
| `vss-motion` | Kuksa poll → `telemetry` records | 10 Hz |
| `vss-imu` | Kuksa poll → `telemetry` records | 10 Hz |
| `vss-power` | Kuksa poll → `telemetry` records | 2 Hz |
| `vss-environment` | Kuksa poll → `telemetry` records | 1 Hz |
| `vss-safety_events` | Kuksa subscribe → `event` records | on change |
| `vss-heartbeats` | Kuksa subscribe → internal state | on change |
| `heartbeat-stm32` | Heartbeat watchdog → `event` on timeout | 1 Hz check |
| `heartbeat-rpi5` | Publishes RPi5 counter to Kuksa | 2 Hz |
| `control-api` | HTTP server for web viewer | on request |

All threads share a single `SessionWriter` instance and its internal `threading.Lock`, so every write is serialised and the JSONL file is always valid.

## 5. Log Format — Record Reference

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

## 6. Configuration

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

## 7. File Layout

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

## 8. Analysis & Visualization

Session files can be analyzed in three ways:

**Command line** — JSONL is grep/jq-friendly. Filtering by record type, extracting a single signal over time, or exporting to CSV for spreadsheet analysis are all one-liners.

**Offline viewer** — A standalone HTML file (no server required) that loads a `.jsonl` session file via drag-and-drop, parses it client-side, and renders interactive Chart.js time-series plots. The viewer can overlay multiple signals, zoom into specific time windows, and highlight events (AEB triggers, heartbeat drops) on the timeline. This file can be opened on any laptop with a browser — no dependencies, no install.

**Live web dashboard** — An NGINX instance (deployed via Docker Compose on port 80) serves the `/data/logs/` directory with CORS headers. The datalogger service maintains a `latest.jsonl` symlink pointing to the current session file. The same HTML viewer, when accessed at `http://<car-ip>/`, polls `latest.jsonl` in live mode, displaying real-time telemetry and events as the car drives — no `scp` needed, any browser on the same network can connect.

```
Live mode:    Browser ──► http://10.21.220.191/ ──► NGINX ──► /data/logs/latest.jsonl
Offline mode: Browser ──► drag-and-drop local .jsonl file
```

Because the viewer is a pure client-side HTML application and the backend is a standard NGINX file server, the same setup scales beyond the local network with minimal effort. Exposing the car's NGINX through a reverse proxy, a VPN, or simply hosting the session files on any public web server would make the dashboard accessible from anywhere — the viewer doesn't care whether the `.jsonl` file comes from `10.21.220.x` or a public URL.

## 9. Deployment Notes

A few issues were encountered and resolved during the initial deployment that are worth documenting for future reference:

**Stale config surviving reflash** — Because `/data/` persists across RAUC updates, an outdated `datalogger_config.yaml` from a previous version survived a system reflash and caused the service to subscribe to VSS paths that no longer existed. The fix was to replace the config in `/data/seame-configs/` with the updated version after the reflash. This is a general consideration for any config stored on the persistent partition.

**VSSClient context manager** — The `kuksa-client` 0.4.x Python library requires `VSSClient` to be used as a context manager (`with VSSClient(...) as client:`), not via a `.connect()` method. Since the datalogger is a long-running service, this required structuring the connection lifecycle inside a dedicated thread that manages the context manager and reconnects on failure.

**TLS certificate handling** — The Kuksa databroker uses TLS with certificates stored at `/etc/kuksa/tls/`. The `VSSClient` expects the CA certificate path as a `Path` object, not raw bytes. The JWT token file must also be read and passed as a string. Both are configured in the `kuksa` section of the YAML config.
