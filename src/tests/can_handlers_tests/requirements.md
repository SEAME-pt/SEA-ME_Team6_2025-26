# SEA:ME — PiRacer CAN → KUKSA Publisher
# Requirements extracted from current implementation

> Format: REQ-<AREA>-<NNN>
> Each requirement includes acceptance criteria that should be validated via GoogleTest.

---

## 1. Frame Dispatching (`dispatch_frames.cpp`)

### REQ-DISPATCH-001 — Dispatch by standard CAN ID
The dispatcher shall match handlers using the standard frame ID extracted as `frame.can_id & CAN_SFF_MASK`.

**Acceptance criteria**
- Given `frame.can_id = (KNOWN_ID | flags)`, the dispatcher calls the correct handler.
- Routing is based only on the masked ID; flags do not affect handler selection.

### REQ-DISPATCH-002 — Known IDs call exactly one handler
For any CAN ID present in the handler table, the dispatcher shall call the corresponding handler exactly once and then return.

**Acceptance criteria**
- Only the matched handler is invoked once.
- No other handlers are invoked after the match.

### REQ-DISPATCH-003 — Unknown IDs are ignored
If the frame CAN ID does not exist in the handler table, the dispatcher shall perform no action.

**Acceptance criteria**
- With an unknown ID, no handler functions are invoked.

---

## 2. KUKSA Client Publishing (`kuksa_client.cpp`)

### REQ-KUKSA-001 — Connect to KUKSA using gRPC insecure channel
The client shall create a gRPC channel using `grpc::CreateChannel(addr, grpc::InsecureChannelCredentials())` and create a `VAL::Stub`.

**Acceptance criteria**
- Constructing the client results in a non-null stub (or equivalent verifiable seam in tests).

### REQ-KUKSA-002 — Publish double values to a VSS path
`publishDouble(path, value)` shall publish a `PublishValueRequest` where:
- `signal_id.path == path`
- `data_point.value.double_ == value`

**Acceptance criteria**
- A fake/stubbed VAL service receives a request with the correct path and oneof field set to `double_`.

### REQ-KUKSA-003 — Publish int32 values to a VSS path
`publishInt32(path, value)` shall publish a `PublishValueRequest` where:
- `signal_id.path == path`
- `data_point.value.int32 == value`

**Acceptance criteria**
- Received request uses the `int32` oneof field with the correct value.

### REQ-KUKSA-004 — Publish bool/float/uint32 values to a VSS path
`publishBool`, `publishFloat`, and `publishUint32` shall set the correct oneof field respectively.

**Acceptance criteria**
- Each method sets only its intended field (`bool_`, `float_`, `uint32`) and preserves the input value.

### REQ-KUKSA-005 — Log on publish failure
If `PublishValue` returns a non-ok status, the client shall write an error to `std::cerr` containing the path and the gRPC error message.

**Acceptance criteria**
- For a forced non-ok status, stderr contains `PublishValue(<path>) failed:` (or equivalent) and includes the error message.

---

## 3. CAN Decode Helpers (`can_decode.cpp`)

### REQ-DECODE-001 — Decode uint8
`u8(p)` shall return `p[0]`.

**Acceptance criteria**
- For `p[0]=0xAB`, the returned value is `0xAB`.

### REQ-DECODE-002 — Decode little-endian int16/uint16
`i16_le(p)` and `u16_le(p)` shall decode 16-bit little-endian values.

**Acceptance criteria**
- `[0x34, 0x12]` decodes to `u16=0x1234`.
- `[0x00, 0x80]` decodes to `i16=-32768`.

### REQ-DECODE-003 — Decode little-endian uint32
`u32_le(p)` shall decode 32-bit little-endian values.

**Acceptance criteria**
- `[0x78,0x56,0x34,0x12]` decodes to `0x12345678`.

### REQ-DECODE-004 — Decode little-endian 24-bit
`u24_le(p)` shall decode 24-bit little-endian into a 32-bit unsigned integer.

**Acceptance criteria**
- `[0x11,0x22,0x33]` decodes to `0x332211`.

### REQ-DECODE-005 — DLC check helper
`dlc_at_least(frame,n)` shall return true iff `frame.can_dlc >= n`.

**Acceptance criteria**
- `can_dlc=7,n=8` → false.
- `can_dlc=8,n=8` → true.

---

## 4. Environment Handler (`environment.cpp`)

### REQ-ENV-001 — Ignore frames with DLC < 8
The environment handler shall perform no publish calls if `frame.can_dlc < 8`.

**Acceptance criteria**
- For DLC values 0..7, zero publish calls occur.

### REQ-ENV-002 — Decode environment payload fields
For DLC ≥ 8, the handler shall decode:
- `raw_temp`: int16 little-endian at bytes 0..1
- `humidity`: uint8 at byte 2
- `pressure`: uint24 little-endian at bytes 4..6

**Acceptance criteria**
- Known byte patterns yield expected decoded values.

### REQ-ENV-003 — Convert and publish environment signals
The handler shall compute:
- `temp_c = raw_temp / 100.0`
- `pressure_kpa = pressure / 1000.0`
and publish:
- `sig::EXT_AIR_TEMPERATURE` as double `temp_c`
- `sig::EXT_HUMIDITY` as float `humidity`
- `sig::EXT_ATMOS_PRESSURE_KPA` as float `pressure_kpa`

**Acceptance criteria**
- Publish calls occur with correct paths, types, and scaled values.

---

## 5. Heartbeat Handler (`heartbeat_stm.cpp`)

### REQ-HB-001 — Ignore frames with DLC < 1
The heartbeat handler shall perform no publish calls if `frame.can_dlc < 1`.

**Acceptance criteria**
- With DLC=0, zero publish calls occur.

### REQ-HB-002 — Publish heartbeat as int32
For DLC ≥ 1, the handler shall read byte0 and publish it as int32 to `sig::ECU_SC_HEARTBEAT`.

**Acceptance criteria**
- For byte0=7, a publishInt32 occurs with value 7 to the correct path.

---

## 6. Speed Handler (`speed.cpp`)

### REQ-SPEED-001 — Ignore frames with DLC < 8
The speed handler shall perform no publish calls if `frame.can_dlc < 8`.

**Acceptance criteria**
- For DLC values 0..7, zero publish calls occur.

### REQ-SPEED-002 — Decode RPM from bytes 0..1
For DLC ≥ 8, RPM shall be decoded from bytes 0..1 as signed int16 little-endian.

**Acceptance criteria**
- Known byte patterns yield expected signed RPM.

### REQ-SPEED-003 — Compute speed from RPM and publish
The handler shall compute:
- `rps = rpm / 60.0`
- `speed_ms = rps * WHEEL_PERIMETER`
and publish `sig::VEHICLE_SPEED` as double `speed_ms`.

**Acceptance criteria**
- With RPM=60 and wheel perimeter `P`, published speed equals `P` m/s.

---

## 7. Battery Handler (`battery.cpp`)

### REQ-BATT-001 — Ignore short frames
If `frame.can_dlc < 8`, the handler shall do nothing.

**Acceptance criteria**
- No publish calls are made when DLC < 8.

### REQ-BATT-002 — Decode and scale voltage and current
For DLC ≥ 8, the handler shall:
- decode `voltage_mv` from bytes 0–1 (u16 LE) and publish `voltage_mv / 1000.0` as float to `sig::TBATT_VOLT_V`
- decode `current_ma` from bytes 2–3 (i16 LE) and publish `current_ma / 1000.0` as float to `sig::TBATT_CURR_A`

**Acceptance criteria**
- Published values match expected scaling for known payloads (including negative current).
- Published type is float and paths are correct.

### REQ-BATT-003 — Publish SOC as float
For DLC ≥ 8, the handler shall decode SOC from byte 4 (u8) and publish it as float to `sig::TBATT_SOC_CURRENT`.

**Acceptance criteria**
- Published SOC equals input byte as a float.


### REQ-BATT-004 — Compute “low” battery flag
For DLC ≥ 8, the handler shall publish `sig::TBATT_IS_LEVEL_LOW` as true if:
- status bit2 is set **OR**
- SOC < 20

Otherwise false.

**Acceptance criteria**
- Low is true for (status bit2 set) even with SOC ≥ 20.
- Low is true for SOC = 19 even if status bit2 is clear.
- Low is false for SOC ≥ 20 and status bit2 clear.


### REQ-BATT-005 — Compute “critical” battery flag
For DLC ≥ 8, the handler shall publish `sig::TBATT_IS_CRITICAL` as true if:
- status bit0 is set **OR**
- SOC < 10

Otherwise false.

**Acceptance criteria**
- Critical is true for (status bit0 set) even with SOC ≥ 10.
- Critical is true for SOC = 9 even if status bit0 is clear.
- Critical is false for SOC ≥ 10 and status bit0 clear.

### REQ-BATT-006 — Publish exactly 5 signals for valid frames
For DLC ≥ 8, the handler shall publish exactly:
- 3 floats (voltage, current, SOC)
- 2 bools (low, critical)

**Acceptance criteria**
- Total publish calls = 5.
- Signal types and VSS paths match expectations.


---

## 8. Emergency Stop Handler (`emergency_stop.cpp`)

### REQ-ESTOP-001 — Ignore frames with DLC < 8
The emergency stop handler shall perform no publish calls if `frame.can_dlc < 8`.

**Acceptance criteria**
- For DLC values 0..7, zero publish calls occur.

### REQ-ESTOP-002 — Publish front distance and warning state
For DLC ≥ 8, the handler shall decode:
- `active = u8 at byte0`
- `dist_mm = u16 little-endian bytes2..3`
and publish:
- `sig::ADAS_FRONT_DISTANCE_MM` as float(dist_mm)
- `sig::ADAS_FRONT_IS_WARNING` as bool(active != 0)

**Acceptance criteria**
- active=1 publishes warning true; distance bytes decode correctly.

---

## 9. IMU Acceleration Handler (`imu_accel.cpp`)

### REQ-ACC-001 — Ignore frames with DLC < 8
The IMU acceleration handler shall perform no publish calls if `frame.can_dlc < 8`.

**Acceptance criteria**
- For DLC values 0..7, zero publish calls occur.

### REQ-ACC-002 — Decode acceleration axes in mg
For DLC ≥ 8, the handler shall decode:
- `ax_mg`: int16 little-endian bytes0..1
- `ay_mg`: int16 little-endian bytes2..3
- `az_mg`: int16 little-endian bytes4..5

**Acceptance criteria**
- Known byte patterns yield expected signed mg values.

### REQ-ACC-003 — Convert mg to m/s² and publish
The handler shall convert per axis:
`ms2 = (mg / 1000.0) * 9.80665`
and publish floats:
- `sig::ACC_LONGITUDINAL` = ax_ms2
- `sig::ACC_LATERAL` = ay_ms2
- `sig::ACC_VERTICAL` = az_ms2

**Acceptance criteria**
- 1000 mg publishes 9.80665 m/s².

---

## 10. IMU Gyro Handler (`imu_gyro.cpp`)

### REQ-GYRO-001 — Ignore frames with DLC < 8
The IMU gyro handler shall perform no publish calls if `frame.can_dlc < 8`.

**Acceptance criteria**
- For DLC values 0..7, zero publish calls occur.

### REQ-GYRO-002 — Decode angular velocity raw (0.1 dps)
For DLC ≥ 8, the handler shall decode:
- `gx_raw`: int16 little-endian bytes0..1
- `gy_raw`: int16 little-endian bytes2..3
- `gz_raw`: int16 little-endian bytes4..5

**Acceptance criteria**
- Known byte patterns yield expected signed raw values.

### REQ-GYRO-003 — Convert to rad/s and publish
The handler shall convert:
- `dps = raw / 10.0`
- `rad_s = dps * (pi/180)`
and publish floats:
- `sig::ANG_VEL_ROLL` = gx_rad_s
- `sig::ANG_VEL_PITCH` = gy_rad_s
- `sig::ANG_VEL_YAW` = gz_rad_s

**Acceptance criteria**
- raw=10 → 1 dps → 0.0174532925 rad/s.

---
