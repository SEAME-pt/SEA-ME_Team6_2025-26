# TSF Validators Guide

Options:
. Manter separados os validators de estrutura vs validators de conteúdo
. Ou criar uma convenção clara (ex: validate_*.py = CI, check_assumption_*.py = trudag)

Optamos pela primeira opção.

## Respostas às suas questões:

### 1. Um script por assumption vs script universal

Existem **duas abordagens** possíveis:

---

## ABORDAGEM A: Um validator universal (RECOMENDADA)

### Características:
- ✅ **Um único script** valida todas as assumptions
- ✅ **Mais fácil de manter** (lógica centralizada)
- ✅ **Configurável** via argumentos ou metadados no YAML
- ✅ **Escalável** para novas assumptions

### Exemplo de estrutura:

```python
#!/usr/bin/env python3
"""Universal validator for all ASSUMPTIONS.

Usage:
    python validate_assumptions.py <assumption-id> <assumption-file>

Examples:
    python validate_assumptions.py ASSUMP-L0-1 items/assumptions/ASSUMP-L0-1.md
    python validate_assumptions.py ASSUMP-L0-2 items/assumptions/ASSUMP-L0-2.md
"""
import sys
from pathlib import Path

def validate_hardware_availability(assumption_file):
    """Valida ASSUMP-L0-1: Hardware components availability"""
    # Lógica específica para hardware
    pass

def validate_hardware_assembly(assumption_file):
    """Valida ASSUMP-L0-2: Hardware assembly prerequisites"""
    # Lógica específica para assembly
    pass

def validate_linux_environment(assumption_file):
    """Valida ASSUMP-L0-3: Linux platform availability"""
    # Lógica específica para Linux
    pass

# Mapeamento: assumption-id -> função validadora
VALIDATORS = {
    'ASSUMP-L0-1': validate_hardware_availability,
    'ASSUMP-L0-2': validate_hardware_assembly,
    'ASSUMP-L0-3': validate_linux_environment,
    # ... adiciona-se mais conforme necessário
}

def main():
    if len(sys.argv) < 3:
        print("Usage: validate_assumptions.py <id> <file>")
        sys.exit(2)
    
    assumption_id = sys.argv[1]
    assumption_file = Path(sys.argv[2])
    
    if assumption_id not in VALIDATORS:
        print(f"No validator configured for {assumption_id}")
        sys.exit(2)
    
    validator_func = VALIDATORS[assumption_id]
    return validator_func(assumption_file)

if __name__ == '__main__':
    main()
```

### Como usar no YAML (ASSUMP-L0-1.md):

```yaml
validators:
- type: script
  command: python3 docs/TSF/tsf_implementation/validators/validate_assumptions.py ASSUMP-L0-1 ${ITEM_FILE}
```

**Nota**: Preciso verificar a sintaxe exata que o trudag espera para validators.

---

## ABORDAGEM B: Um script por assumption

### Características:
- ✅ **Isolamento total** entre validações
- ✅ **Simplicidade** (cada script é independente)
- ❌ **Mais difícil de manter** (código duplicado)
- ❌ **Proliferação de arquivos** (17 scripts para 17 assumptions)

### Estrutura:

```
validators/
├── validate_all.py                    # CI validator (já existe)
├── validate_front_matter.py           # CI validator (já existe)
├── validate_extended.py               # CI validator (já existe)
├── validate_assump_l0_1_hardware.py   # Trudag validator
├── validate_assump_l0_2_assembly.py   # Trudag validator
├── validate_assump_l0_3_linux.py      # Trudag validator
└── ... (mais 14 scripts)
```

### Como usar no YAML:

```yaml
# ASSUMP-L0-1.md
validators:
- type: file
  path: ../../validators/validate_assump_l0_1_hardware.py

# ASSUMP-L0-2.md
validators:
- type: file
  path: ../../validators/validate_assump_l0_2_assembly.py
```

---

## COMPARAÇÃO

| Critério | Universal (A) | Individual (B) |
|----------|---------------|----------------|
| Manutenção | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| Clareza | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Escalabilidade | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| Isolamento | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Reuso de código | ⭐⭐⭐⭐⭐ | ⭐ |

**Recomendação**: Usar **Abordagem A (Universal)** pois é mais escalável e fácil de manter.

---

## 2. Quando executam os validators de CI?

### Workflow atual: `tsf-validate.yml`

```yaml
name: 'TSF Front-matter validation'

on:
  pull_request:
    branches: [ development, main ]  # ← Executa em PRs para development ou main
  push:
    branches: [ development ]         # ← Executa em pushes diretos para development

jobs:
  validate-tsf:
    runs-on: ubuntu-latest
    steps:
      - name: Run TSF validators
        run: |
          python3 docs/TSF/tsf_implementation/validators/validate_all.py
```

### Quando executa:

1. **Pull Request para `development` ou `main`**
   - Alguém abre um PR
   - GitHub Actions executa `validate_all.py`
   - Se falhar → PR fica bloqueado (depende das regras do repo)

2. **Push direto para `development`**
   - Alguém faz push direto (sem PR)
   - GitHub Actions executa `validate_all.py`
   - Se falhar → notificação, mas código já está no branch

3. **Manualmente** (quando quiser)
   - Desenvolvedor executa localmente:
     ```bash
     python3 docs/TSF/tsf_implementation/validators/validate_all.py
     ```

### Diferença fundamental:

```
┌─────────────────────────────────────────────────────────────┐
│  VALIDATORS DE CI (validate_all.py)                         │
│  ├─ Quando: PRs, pushes, manual                             │
│  ├─ O quê: Verifica ESTRUTURA e FORMATO                     │
│  └─ Efeito: Bloqueia PR se falhar (não afeta scores)        │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│  VALIDATORS DO TRUDAG (validate_assumptions.py)             │
│  ├─ Quando: trudag manage score                             │
│  ├─ O quê: Verifica CONTEÚDO e CLAIMS                       │
│  └─ Efeito: Calcula score da ASSUMPTION (0.0 a 1.0)         │
└─────────────────────────────────────────────────────────────┘
```

---

## Próximos passos recomendados:

1. **Criar validator universal** (`validate_assumptions.py`)
2. **Adicionar lógica para ASSUMP-L0-1** (hardware)
3. **Testar com trudag score**
4. **Se funcionar, adicionar ASSUMP-L0-2, L0-3**, etc.
5. **(Opcional) Adicionar ao CI** se quiser que validators do trudag também bloqueiem PRs

---

## Perguntas antes de avançar:

1. Prefere **Abordagem A (universal)** ou **B (individual)**?
2. Quer que eu verifique a **sintaxe exata** que o trudag espera para validators?
3. Quer adicionar os **validators do trudag no CI** também?
