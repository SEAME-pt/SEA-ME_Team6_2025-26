# Technical Spike — Lateral Control: MPC vs PID vs Pure Pursuit (SEA:ME Team 6)

> Date: 08/05/2026  
> Branch: `spike/MPC`  
> Authors: SEA:ME Team 6 — 42 Porto  
> Context: Integration with the existing lane detection + steering pipeline running on AGL

---

## Table of Contents

1. [Objective of the Spike](#1-objective-of-the-spike)  
2. [Current System State (PID on AGL)](#2-current-system-state-pid-on-agl)  
   - 2.1 [Architecture Overview](#21-architecture-overview)  
   - 2.2 [Analysis of `lka_steering_v1_2.py`](#22-analysis-of-lka_steering_v1_2py)  
   - 2.3 [What Works Well (Reusable for MPC)](#23-what-works-well-reusable-for-mpc)  
   - 2.4 [Known Limitations of the Current PID](#24-known-limitations-of-the-current-pid)  
3. [PID — Concept and Role](#3-pid--concept-and-role)  
4. [MPC — Concept and Application](#4-mpc--concept-and-application)  
   - 4.1 [What is MPC?](#41-what-is-mpc)  
   - 4.2 [How MPC would work with this system](#42-how-mpc-would-work-with-this-system)  
   - 4.3 [MPC Cost Function (example)](#43-mpc-cost-function-example)  
5. [Pure Pursuit — Geometric Alternative](#5-pure-pursuit--geometric-alternative)  
   - 5.1 [What is Pure Pursuit?](#51-what-is-pure-pursuit)  
   - 5.2 [Applicability to this system](#52-applicability-to-this-system)  
6. [Comparative Analysis: PID vs MPC vs Pure Pursuit](#6-comparative-analysis-pid-vs-mpc-vs-pure-pursuit)  
7. [Integration Strategy — How to reuse `lka_steering_v1_2.py`](#7-integration-strategy--how-to-reuse-lka_steering_v1_2py)  
   - 7.1 [Reusable components](#71-reusable-components)  
   - 7.2 [Proposed unified controller interface](#72-proposed-unified-controller-interface)  
   - 7.3 [Incremental rollout architecture](#73-incremental-rollout-architecture)  
8. [Experimental Plan (Spike Implementation)](#8-experimental-plan-spike-implementation)  
9. [Evaluation Metrics and Decision Criteria](#9-evaluation-metrics-and-decision-criteria)  
10. [Risks and Mitigation](#10-risks-and-mitigation)  
11. [Recommended Decision for this Sprint](#11-recommended-decision-for-this-sprint)  
12. [Next Concrete Steps](#12-next-concrete-steps)  

---

## 1) Objective of the Spike

The goal of this spike is to evaluate whether **Model Predictive Control (MPC)** can improve lateral steering performance compared to the current **PID controller** already running well on the AGL car, and to also investigate **Pure Pursuit** as a third option.

The key questions this spike answers:

- What are the conceptual and practical differences between PID, MPC, and Pure Pursuit?
- What components of the existing code (`lka_steering_v1_2.py`) can be directly reused?
- How would MPC integrate with the current Kuksa + CAN pipeline?
- Is there a clear performance gain that justifies the added complexity?
- What is the recommended strategy for this sprint?

---

## 2) Current System State (PID on AGL)

### 2.1 Architecture Overview

The current pipeline on the AGL target (`10.21.220.191`) is:

```
Camera
  └─► rc_inference-with-kuksa.py   (UFLDv2 lane detection → publishes LateralDeviation to Kuksa)
        └─► kuksa.val (gRPC)
              └─► lka_steering_v1_2.py  (subscribes → PID → CAN → car)
                    └─► CAN bus (can1, 0x500)
                          └─► Physical steering + throttle
```

The perception and control are **decoupled via Kuksa signals**:
- `Vehicle.ADAS.LaneKeepAssist.LateralDeviation` — normalised lateral offset from lane center
- `Vehicle.ADAS.LaneKeepAssist.LaneStatus` — `"both"`, `"left"`, `"right"`, `"none"`

### 2.2 Analysis of `lka_steering_v1_2.py`

The controller implements a clean 8-step pipeline per frame:

| Step | Operation | Description |
|------|-----------|-------------|
| 0 | Lane status check | If `"none"` → hold last steering, skip PID |
| 1 | Invert (optional) | `--invert` flag flips deviation sign |
| 2 | EMA smoothing | `smooth = α × raw + (1−α) × old` — removes detection noise |
| 3 | Deadband | Deviations below threshold → treated as 0 (avoids jitter near center) |
| 4 | PID | `P + I + D` on error = `0 − smoothed_deviation` |
| 5 | Center snap | Tiny PID outputs → forced to 0 (avoids micro-corrections) |
| 6 | Clamp | Output clamped to `[−100, +100]` |
| 7 | Rate limit | Max steering change per frame (`--max-rate`, default 20) |
| 8 | CAN send | `struct.pack('<hh', steering, throttle)` → CAN ID `0x500` |

**Key parameters (with defaults):**

```
--kp          3.0     Proportional gain
--ki          0.0     Integral gain (disabled by default!)
--kd          3.0     Derivative gain
--smoothing   0.5     EMA alpha (0.1=very smooth, 1.0=no smoothing)
--deadband    0.08    Ignore deviations below this
--snap        3.0     PID outputs below this → steering=0
--throttle    0       Throttle CAN value
--max-rate    20      Max steering change per step
--invert      False   Flip deviation sign
```

> **Note:** Ki is 0.0 by default — the controller is effectively a **PD controller**, not a full PID.
> This is a deliberate and sound choice: integral windup in a fast lane-keeping loop can cause instability.

### 2.3 What Works Well (Reusable for MPC)

The following components are **directly reusable** regardless of the control algorithm:

| Component | Reuse in MPC? | Notes |
|-----------|--------------|-------|
| Kuksa gRPC subscriber | ✅ Yes | No changes needed — MPC reads same signals |
| `LaneStatus` hold logic (step 0) | ✅ Yes | Critical safety feature — keep as-is |
| EMA smoothing (step 2) | ✅ Yes | MPC also benefits from noise-free input |
| Deadband (step 3) | ✅ Yes | Reduces unnecessary micro-corrections |
| CAN send `_send()` (step 8) | ✅ Yes | Identical output format |
| Rate limiter (step 7) | ⚠️ Partial | MPC has native rate constraints — can be kept as safety clip |
| Argparse structure | ✅ Yes | Extend with MPC-specific params |

### 2.4 Known Limitations of the Current PID

| Limitation | Impact | MPC improvement |
|-----------|--------|----------------|
| Reacts to current error only | Oscillation in sharp curves (90°) | MPC predicts ahead → smoother entry |
| No curvature feedforward | Late reaction to tight turns | MPC can include curvature as input |
| Ki=0 by default → no steady offset correction | Small persistent lateral drift possible | MPC cost function handles this natively |
| Rate limiting is post-hoc | Abrupt corrections may still occur | MPC rate constraint is part of optimization |
| No explicit steering bounds in cost | May over-correct | MPC hard constraints on `u ∈ [−100, +100]` |

---

## 3) PID — Concept and Role

**PID (Proportional-Integral-Derivative)** computes a control output based on three terms:

```
u(t) = Kp × e(t)  +  Ki × ∫e(t)dt  +  Kd × de/dt
```

Where `e(t)` = target − current = `0 − lateral_deviation`.

**Strengths:**
- Extremely simple and fast to compute
- Easy to tune manually
- Very low latency

**Weaknesses:**
- Reactive only (no prediction)
- No explicit constraint handling
- Sensitive to noise in derivative term (mitigated here by EMA + snap)

**Role going forward:**  
PID remains the **production baseline** and the A/B reference. It should not be removed.

---

## 4) MPC — Concept and Application

### 4.1 What is MPC?

**Model Predictive Control** solves, at each timestep, a finite-horizon optimization problem:

```
minimize   Σ [ w_y × e_y(k)²  +  w_ψ × e_ψ(k)²  +  w_u × u(k)²  +  w_Δu × Δu(k)² ]
subject to:
  x(k+1) = f(x(k), u(k))   [vehicle model]
  u(k) ∈ [−100, +100]       [steering limits]
  Δu(k) ∈ [−max_rate, +max_rate]  [rate limits]
```

Where:
- `e_y` = lateral error
- `e_ψ` = heading error (angle)
- `u` = steering command
- `Δu` = change in steering
- `N` = prediction horizon (e.g., 5–15 steps)

Only the **first control action** `u(0)` is applied. The rest is discarded and recalculated next frame.

### 4.2 How MPC would work with this system

MPC inputs from existing Kuksa signals:

| Signal | Already available? | Usage in MPC |
|--------|-------------------|-------------|
| `LateralDeviation` | ✅ Yes | `e_y` — lateral error |
| `LaneStatus` | ✅ Yes | Safety guard (hold if "none") |
| Heading error `e_ψ` | ❌ Not yet | Needs to be published by inference script |
| Curvature | ❌ Not yet | Optional but very useful as feedforward |
| Vehicle speed | ❌ Not yet | Needed for accurate model |

**Minimum viable MPC** only requires `LateralDeviation` (same as PID), using a simplified kinematic model.

**Full MPC** would also need heading error and speed — achievable by extending `rc_inference-with-kuksa.py`.

### 4.3 MPC Cost Function (example)

A minimal cost function for this system:

```python
# Horizon N=10, state: [e_y, e_psi], input: steering
J = 0
for k in range(N):
    J += w_y   * e_y[k]**2      # penalise lateral offset
    J += w_psi * e_psi[k]**2    # penalise heading error
    J += w_u   * u[k]**2        # penalise large steering angles
    J += w_du  * (u[k] - u[k-1])**2  # penalise sudden changes (smoothness)
```

Suggested initial weights: `w_y=10, w_psi=5, w_u=1, w_du=5`.

---

## 5) Pure Pursuit — Geometric Alternative

### 5.1 What is Pure Pursuit?

Pure Pursuit is a **geometric path-following** algorithm:

1. Define a **lookahead point** on the reference path at distance `L_d` ahead.
2. Compute the steering angle to drive to that point:

```
δ = arctan( 2 × L × sin(α) / L_d )
```

Where:
- `L` = vehicle wheelbase
- `L_d` = lookahead distance (tuning parameter)
- `α` = angle between heading and lookahead point direction

**Strengths:**
- Very simple and computationally cheap
- Smooth output by nature (geometric, not reactive)
- Good at following smooth curves

**Weaknesses:**
- Requires a well-defined path/centerline (not just a deviation value)
- Lookahead distance `L_d` is speed-dependent (must scale with velocity)
- Sensitive to lane detection noise at the lookahead point

### 5.2 Applicability to this system

| Requirement | Available? | Notes |
|-------------|-----------|-------|
| Lateral deviation | ✅ Yes | From Kuksa |
| Path centerline / lookahead point | ❌ Not yet | Requires richer output from inference |
| Vehicle heading | ❌ Not yet | Needed for angle `α` |
| Vehicle speed | ❌ Not yet | For dynamic `L_d` scaling |

**Conclusion:** Pure Pursuit is not directly applicable with the current minimal signal set. It becomes interesting in a **Phase 2** where the inference script publishes a fuller set of lane geometry signals.

---

## 6) Comparative Analysis: PID vs MPC vs Pure Pursuit

| Criterion | PID (current) | MPC | Pure Pursuit |
|-----------|--------------|-----|-------------|
| Complexity | Low | Medium/High | Low/Medium |
| Tuning effort | Low (fast) | Medium (more params) | Low |
| Immediate reaction | ✅ Very good | ✅ Good | ✅ Good |
| Curve anticipation | ❌ No | ✅ Yes (horizon) | ⚠️ Partial (lookahead) |
| Explicit constraints (rate, limits) | ❌ No | ✅ Yes (native) | ❌ No |
| Handles noise | ⚠️ Via EMA+snap | ⚠️ Via cost function | ❌ Sensitive |
| Computational cost | Very low | Higher (QP solver) | Very low |
| Required inputs | `e_y` only | `e_y` (+`e_ψ`, v) | Path + heading |
| Integration effort with current stack | ✅ Already done | Medium | High |
| Risk for this sprint | None (baseline) | Low/Medium | High |
| Recommended role | Production | Shadow mode → A/B test | Future phase |

---

## 7) Integration Strategy — How to reuse `lka_steering_v1_2.py`

### 7.1 Reusable components

From the analysis in §2.3, the following can be extracted into a shared base class:

```python
# Reusable regardless of controller:
# - KuksaSubscriber (gRPC connection + Subscribe loop)
# - EMA smoother
# - LaneStatus hold logic
# - CAN sender (_send method)
# - Rate limiter (as safety clip)
# - Argparse skeleton
```

### 7.2 Proposed unified controller interface

A clean abstraction that preserves the existing logic while allowing controller swapping:

```python
class BaseController:
    """Abstract base — implemented by PID, MPC, PurePursuit."""
    def compute(self, e_y: float, e_psi: float, v: float, now: float) -> float:
        raise NotImplementedError

class PIDController(BaseController):
    # Exact same logic as current lka_steering_v1_2.py PID class
    ...

class MPCController(BaseController):
    # New: solves QP over horizon N
    ...

class PurePursuitController(BaseController):
    # New: geometric lookahead (requires richer lane signal)
    ...
```

Selection via environment variable or CLI flag:

```bash
sudo python3 lka_steering_v2.py --controller pid      # current behaviour
sudo python3 lka_steering_v2.py --controller mpc      # new
sudo python3 lka_steering_v2.py --controller pursuit  # future
```

### 7.3 Incremental rollout architecture

```
Phase 1 (now):      PID → production + logging
Phase 2 (spike):    MPC runs in "shadow mode" (computes but does NOT send to CAN)
                    → compare PID output vs MPC output in logs
Phase 3:            A/B testing: same course, PID vs MPC, measure RMSE
Phase 4:            Hybrid: PID on straights, MPC on curves (curvature threshold)
Phase 5 (optional): Pure Pursuit as smooth fallback on known map segments
```

---

## 8) Experimental Plan (Spike Implementation)

### Day 1 — Setup and shadow mode
- [ ] Extract base class from `lka_steering_v1_2.py` (Kuksa, CAN, EMA, rate limiter)
- [ ] Implement minimal `MPCController` (horizon N=10, QP with `scipy.optimize` or `casadi`)
- [ ] Run MPC in shadow mode alongside PID on real hardware
- [ ] Log both outputs to CSV: `[timestamp, raw_dev, smooth_dev, pid_steer, mpc_steer, lane_status]`

### Day 2 — Evaluation and decision
- [ ] Run 3 identical laps: PID only, MPC only, hybrid
- [ ] Compute metrics (see §9)
- [ ] Plot steering signals: look for oscillation, overshoot, smoothness
- [ ] Document decision with real numbers
- [ ] Update this document with results

---

## 9) Evaluation Metrics and Decision Criteria

| Metric | Formula | Target |
|--------|---------|--------|
| RMSE lateral error | `√(mean(e_y²))` | Lower is better |
| Mean heading error | `mean(|e_ψ|)` | Lower is better |
| % time in-lane | `frames with |e_y| < threshold / total` | > 95% |
| Steering jitter | `mean(|Δu|)` per frame | Lower is better |
| Control loop latency | `t_send − t_recv` | < 20 ms |
| Overshoot in 90° curve | `max(|e_y|)` in curve segment | Lower is better |

**Adoption criterion for MPC:**  
> MPC is adopted if RMSE lateral error improves ≥ 15% vs PID **and** control loop latency stays under 20 ms.

---

## 10) Risks and Mitigation

| Risk | Probability | Impact | Mitigation |
|------|------------|--------|-----------|
| MPC too slow for real-time loop | Medium | High | Use short horizon (N≤10), pre-warm solver, fallback to PID |
| QP solver instability | Low | High | Warm start, feasibility check, fallback to PID on failure |
| Heading error signal not available | High (now) | Medium | Use PID-only for lateral; add heading to Kuksa in parallel |
| Tuning MPC weights takes too long | Medium | Medium | Start with symmetric weights; use shadow mode to compare before committing |
| Pure Pursuit not viable without path | High (now) | Low | Defer to Phase 2; no risk to current sprint |

---

## 11) Recommended Decision for this Sprint

1. **Do NOT replace PID** — it works well and is the production baseline.
2. **Implement MPC in shadow mode** alongside existing PID — no risk to car operation.
3. **Extend `rc_inference-with-kuksa.py`** to also publish heading error (`e_ψ`) to Kuksa — unlocks better MPC and Pure Pursuit in future.
4. **Run A/B test** with logging. If MPC shows clear improvement in curves, advance to hybrid mode.
5. **Pure Pursuit** — defer to a later sprint when richer lane geometry signals are available.

---

## 12) Next Concrete Steps

- [ ] SSH into AGL (`ssh root@10.21.220.191`) and review `/data/lane-detection/rc_inference-with-kuksa.py` to identify what additional signals (heading, curvature) can be computed and published.
- [ ] Refactor `lka_steering_v1_2.py` to extract base class + plugin interface (PID, MPC, Pure Pursuit).
- [ ] Implement minimal `MPCController` using `scipy.optimize.minimize` (no external solver needed for MVP).
- [ ] Add `--controller` flag and CSV logging.
- [ ] Run shadow-mode test for at least 5 minutes on real hardware.
- [ ] Record and compare metrics. Update §9 with results.
- [ ] Decide: keep PID / adopt MPC / adopt hybrid.