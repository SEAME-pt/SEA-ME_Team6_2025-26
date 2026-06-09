# Mobility Scenarios (Module 3)

This folder contains the implementation and validation guides for Module 3 mobility scenarios.

## Scope

- `v2i/`: Vehicle-to-Infrastructure communication with barriers.
- `emergency_priority/`: emergency vehicle priority with barriers and traffic lights.
- `shared/`: common architecture decisions, interfaces, and test strategy.

## Working Branch

- `feature/mobility_scenarios/V2I_and_emergencypriority`

## Delivery Plan (High-Level)

1. Define use cases and requirements (functional + safety constraints).
2. Define message contracts and signal mapping (CAN and KUKSA/VSS).
3. Implement backend services and infrastructure simulators.
4. Integrate with vehicle stack (STM32 -> CAN -> Pi5/AGL -> KUKSA -> UI).
5. Validate with test scenarios and collect evidence for TSF.
6. Prepare demo and sprint closure documentation.

## Sprint 15 Focus (Joao)

- Vehicle-to-Infrastructure: communication with barriers.
- Emergency Vehicle Priority: communication with barriers and traffic lights.
