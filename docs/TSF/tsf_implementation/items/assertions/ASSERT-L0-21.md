---
id: ASSERT-L0-21
header: Complete TSF Workflow Operational
text: 'The unified TSF automation script (open_check_sync_update_validate_run_publish_tsfrequirements.py)
  is operational and executes the complete workflow: open_check (validation), sync_update
  (AI-assisted content generation), and validate_run_publish (TruDAG execution). All
  three phases complete successfully.

  '
level: '1.21'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-21.md
- type: file
  path: ../evidences/EVID-L0-21.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
    - STM32
    - CAN
    - Raspberry Pi
---

