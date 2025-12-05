**TSF Implementation & trudag integration**

Purpose: document how to create and maintain the Trustable Software Framework (TSF)
for this repository and how to operate `trudag` locally and in CI to produce a
trustable report.

Quick checklist
- **Init dotstop**: `trudag init --root docs/TSF/tsf_implementation --output .dotstop.dot`
- **Create items**: `trudag manage create-item --type expectation ...` (see examples below)
- **Create links**: `trudag manage create-link --from <item> --to <item> --type traces` 
- **Set item state**: `trudag manage set-item --id <id> --state validated`
- **Lint graph**: `trudag manage lint --dotstop .dotstop.dot`
- **Score & publish**: `trudag score --dotstop .dotstop.dot && trudag publish --dotstop .dotstop.dot`

Notes & local workflow
- The repository contains a `.dotstop.dot` at the repo root that points to
  `docs/TSF/tsf_implementation`. You can regenerate it using `trudag init` which
  will compute reproducible SHAs for referenced files.
- We keep `EVID`/`ASSERT`/`EXPECT` items under `docs/TSF/tsf_implementation/items/`.

Suggested commands (local)
1. Create a virtualenv and install trudag & gravitz (if needed):

```bash
python3 -m venv .trudag-venv
source .trudag-venv/bin/activate
pip install trudag gravitz
```

2. Initialize dotstop (reproducible SHAs):

```bash
trudag init --root docs/TSF/tsf_implementation --output .dotstop.dot
```

3. Create items from the LO table (example):

```bash
trudag manage create-item --type expectation --id EXPECT-L0-01 --title "Expectation L0-1" --text "..."
trudag manage create-item --type evidence --id EVID-L0-01 --title "Evidence L0-1" --text "..."
```

4. Create links (assertions/traces):

```bash
trudag manage create-link --from EXPECT-L0-01 --to EVID-L0-01 --type traces
trudag manage create-link --from EXPECT-L0-01 --to ASSERT-L0-01 --type supports
```

5. Validate & lint locally:

```bash
trudag manage set-item --id EXPECT-L0-01 --state validated
trudag manage lint --dotstop .dotstop.dot
```

6. Score and publish (produces the trustable report):

```bash
trudag score --dotstop .dotstop.dot
trudag publish --dotstop .dotstop.dot
```

CI notes
- The included GitHub Actions workflow `trudag-report.yml` installs Python,
  installs `trudag`, runs the basic validators, then runs `trudag init` and
  `trudag manage` commands to produce the dotstop and score. It is a template —
  review and adapt secrets and access tokens before enabling publishing.

Where to start
- Run the `trudag init` command locally and verify `.dotstop.dot` contents and
  SHAs. If you want, I can run a dry-run report generation steps here (I won't
  publish to remote without your consent).
