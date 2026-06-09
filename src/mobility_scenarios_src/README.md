# Mobility Scenarios Source (Module 3)

This folder contains source code for Module 3 mobility scenarios.

## Structure

- `v2i/`: barrier communication services and adapters.
- `emergency_priority/`: emergency priority orchestration.
- `shared/`: common models, config, and utilities.

## Integration Context

- Vehicle architecture: STM32 -> CAN -> Pi5/AGL -> KUKSA/VSS -> Qt cluster.
- Scope includes simulation-first implementation and integration hooks.
- The first MVP uses direct local Python service calls; MQTT is optional later.
