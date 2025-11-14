# TSF Workflow

This is an example of a workflow for TSF.

1. Create virtual env  
    ```bash
    python3.12 -m venv .venv
    source .venv/bin/activate
    ```

2. Install trudag (official TSF tool)  
    ```bash
    git clone https://gitlab.com/CodethinkLabs/trustable/trustable.git /tmp/trustable
    cd /tmp/trustable
    git checkout v2025.09.16
    pip install .
    cd -
    ```

3. trudag --init & Create requirement  
    ```bash
    trudag --init
    trudag manage create-item UR 001 reqs/
    # Edit UR-001.md - change the template
    # -> do this for every requirement (UR, SYSR, SOFR, TC)
    trudag manage lint
    ```

4. Link requirements  
    ```bash
    trudag manage create-link UR-001 SYSR-001
    # -> do this for every requirement
    trudag manage lint
    ```

5. Review requirements  
    ```bash
    trudag manage set-item XX-001
    # Review every requirement needed
    ```

6. Add references/evidence & configure scoring  
    - Add reference files to the requirements/tests  
    - The files must exist in the correct path  

    ```bash
    trudag manage lint
    trudag score
    ```

---

# Requirement template:

```yaml
---
id: UR-001
normative: true
level: 1.1

references:
  - type: "file"
    path: "ref1.md"
---

---
id: UR-001
normative: true
level: 1.2

references:
  - type: "file"
    path: "ref2.md"

reviewers:
  - name: David
    email: david.fernandes@seame.pt
    reviewed: "2025-11-12 - Approved by David david.fernandes@seame.pt"
    review_status: true

score:
  David: 0.2
  Joao: 0.4
---
