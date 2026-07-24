# Shared Mobility Scenario Components

Common artifacts shared by V2I and Emergency Priority:

- Data models and enums.
- Configuration schema.
- Timeouts and retry policies.
- Logging and trace correlation conventions.
- Shared micro:bit firmware:
	- [microbit_gateway_makecode.js](microbit_gateway_makecode.js)
	- [microbit_trafficlight_tx_makecode.js](microbit_trafficlight_tx_makecode.js)

Scenario-specific micro:bit firmware stays outside shared:
- Emergency only:
	- barrier firmware
	- streetlight firmware
