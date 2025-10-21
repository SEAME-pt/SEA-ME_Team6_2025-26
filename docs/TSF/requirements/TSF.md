# 🧱 TSF Requirement Levels — Overview and Structure

In the **Trustable Software Framework (TSF)** and many structured software engineering methodologies, there are typically **multiple levels of requirements**, not just **L0** and **L1**.  
Each level adds more **technical detail**, **traceability**, and **verification precision**.

---

## 🔹 1. L0 — Client Requirements (High-level / User-oriented)

- Written from the **client’s perspective**.  
- Describe **what** the system should do in **plain language**.  
- Must include **measurable acceptance criteria**.

**Example (PiRacer):**
> “The GUI shall display static graphical elements on the PiRacer’s built-in screen.”

---

## 🔹 2. L1 — Software / Technical Requirements

- Translate L0 into **technical, implementable requirements**.  
- Specify **how** the system achieves the L0 goals.  
- Include measurable, testable, and traceable criteria.

**Example (PiRacer):**
> “The GUI application shall launch a Qt executable on the PiRacer screen and render at least one static graphical element.”

---

## 🔹 3. L2 — Component / Module Requirements

- Further decomposes L1 into **specific components or modules**.  
- Defines the **behavior, interface, and constraints** of each module.  
- Often includes **input/output specifications**, **performance limits**, and **safety constraints**.

**Example (PiRacer):**
> “The DisplayController module shall provide a `drawRectangle(x, y, width, height)` function to render rectangles on the screen.”

---

## 🔹 4. L3 — Implementation / Detailed Design Requirements

- The **lowest-level**, highly detailed requirements.  
- Focused on **exact algorithms, data structures, and code-level interfaces**.  
- Includes **API specs**, **function signatures**, **configuration parameters**, and **timing requirements**.

**Example (PiRacer):**
> “The `drawRectangle()` function must execute in less than 50 ms for a 128×128 pixel rectangle and use the QPainter API from Qt6.”

---

## 🔹 5. Test Cases (TSF Verification Layer)

> Not a separate “L level”, but tightly linked to all requirement levels.

- Every L0/L1/L2/L3 requirement should have **at least one test case verifying it**.  
- Ensures **traceability and compliance** throughout the software lifecycle.  

---

## ✅ Summary Table

| **Level** | **Focus** | **Example (PiRacer)** |
|:-----------|:-----------|:----------------------|
| **L0** | Client / User | GUI displays static elements |
| **L1** | Software / Technical | Qt GUI launches and renders at least one element |
| **L2** | Component / Module | DisplayController provides `drawRectangle()` |
| **L3** | Implementation / Design | `drawRectangle()` executes in < 50 ms using QPainter |
| **Tests** | Verification | Automated Python script checks GUI launch and rendering |

---

## 🚗 Applying to Your PiRacer Project

For your **PiRacer Warm-Up Project**, you’ve already defined:
- **L0 Requirements** → Client-level goals  
- **L1 Requirements** → Software-level implementations  
- **Test Scripts** → Verification layer  

If you wish to extend your **TSF traceability**, you can define **L2 Component-Level Requirements** for:
- 🖥️ Display Module  
- ⚙️ Motor Control Module  
- 🎮 Remote Input Module  
- 🧩 GUI Module  

This ensures **full end-to-end traceability** from **L0 → L3** and strengthens your **TSF compliance**.
