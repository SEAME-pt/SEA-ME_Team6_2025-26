# TSF Implementation — Sea:ME (official-format)

This folder contains the Trustable Software Framework implementation for the Sea:ME project.
This implementation follows the Trustable/Trudag upstream format (fields: `id`, `header`, `text`, `level`, `normative`, `references`, `reviewers`, etc.) to ensure compatibility with `trudag`/`dotstop`.

Layout
- `items/expectations/` — Expectation items (`EXPECT-L0-*.md`)
- `items/assertions/` — Assertions (`ASSERT-L0-*.md`)
- `items/assumptions/` — Assumptions (`ASU-*.md`)
- `items/evidences/` — Evidence items (`EVID-L0-*.md`)
- `graph/graph.dot` — editable Graphviz DOT representing trace links
- `scripts/generate_from_requirements.py` — generator to create/update items from `LO_requirements.md`
- `scripts/run_trudag_publish.sh` — wrapper to run `trudag` commands to create links, resolve, and publish
- `validators/` — simple validators (front-matter, evidence path/hash)

Guiding principle
- Items are written in English and follow the upstream Trustable schema so `trudag` can manage/lint/score/publish the graph.

Next steps
1. Run the generator to create EXPECT/ASSERT/EVID skeletons from `docs/TSF/requirements/LO_requirements.md`:

```bash
python3 docs/TSF/tsf_implementation/scripts/generate_from_requirements.py
```

2. Review generated items and then run the `trudag` publish flow (requires `trudag` installed):

```bash
bash docs/TSF/tsf_implementation/scripts/run_trudag_publish.sh
```

See `TSF_IMPLEMENTATION.md` for detailed commands and CI configuration.
