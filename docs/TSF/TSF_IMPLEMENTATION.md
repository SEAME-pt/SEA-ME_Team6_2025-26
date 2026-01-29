
# TSF Implementation & trudag integration

Toda a documentação e instruções detalhadas sobre a automatização, geração e integração TSF estão centralizadas em:

👉 [`tsf_implementation/TSF_docs/automatization_scripts_explanation.md`](tsf_implementation/TSF_docs/automatization_scripts_explanation.md)

---

## Resumo rápido

- Para criar, sincronizar e validar os artefatos TSF, utilize o script principal:

```bash
python3 docs/TSF/tsf_implementation/scripts/sync_tsf_requirements_table.py
```

- Para reconstruir a base trudag:

```bash
bash docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh
```

Consulte o documento principal para detalhes, troubleshooting e fluxos completos.
