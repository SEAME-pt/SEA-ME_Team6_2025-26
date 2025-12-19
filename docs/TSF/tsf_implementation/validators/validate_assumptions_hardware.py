#!/usr/bin/env python3
"""Validator for ASSUMP-L0-1: Hardware availability assumption.

Este validator verifica se os componentes de hardware mencionados na assumption
estão documentados no repositório através de:
1. Referências em arquivos de documentação
2. Evidências em imagens de demonstração
3. Menções em guias de instalação

O validator retorna:
- Exit code 0: Todos os componentes estão validados (score = 1.0)
- Exit code 1: Alguns componentes faltam evidências (score parcial)
- Exit code 2: Nenhuma evidência encontrada (score = 0.0)

Como funciona:
- Define lista de componentes esperados
- Procura por menções desses componentes em arquivos do repositório
- Calcula score baseado na % de componentes validados
- Gera relatório detalhado

Para usar com trudag:
1. Este script deve ser referenciado na ASSUMPTION como um validator
2. O trudag executará este script e usará o exit code para determinar o score
3. Adicione na ASSUMP-L0-1.md:
   validators:
   - type: file
     path: ../../validators/validate_assumptions_hardware.py
"""
import sys
import re
from pathlib import Path
from typing import List, Tuple, Dict

# Repositório base (3 níveis acima deste script)
REPO_ROOT = Path(__file__).resolve().parents[3]

# Componentes de hardware esperados na ASSUMP-L0-1
HARDWARE_COMPONENTS = [
    "Expansion Board",
    "Raspberry Pi 5",
    "Hailo AI Hat",
    "DC motors",
    "servo motor",
    "display interface"
]

# Diretórios onde procurar evidências
SEARCH_PATHS = [
    REPO_ROOT / "docs" / "demos",
    REPO_ROOT / "docs" / "guides",
    REPO_ROOT / "docs" / "sprints",
    REPO_ROOT / "src",
]


def search_component_mentions(component: str) -> List[Path]:
    """Procura por menções de um componente nos arquivos do repositório.
    
    Args:
        component: Nome do componente a procurar
        
    Returns:
        Lista de arquivos que mencionam o componente
    """
    found_in = []
    
    # Normaliza o componente para busca (case-insensitive, remove espaços extras)
    search_pattern = re.compile(re.escape(component), re.IGNORECASE)
    
    for search_path in SEARCH_PATHS:
        if not search_path.exists():
            continue
            
        # Procura em arquivos markdown e texto
        for ext in ['*.md', '*.txt', '*.py', '*.sh']:
            for filepath in search_path.rglob(ext):
                try:
                    content = filepath.read_text(encoding='utf-8', errors='ignore')
                    if search_pattern.search(content):
                        found_in.append(filepath)
                except Exception:
                    # Ignora arquivos que não conseguimos ler
                    continue
    
    return found_in


def check_image_evidence(component: str) -> List[Path]:
    """Verifica se há imagens que podem evidenciar o componente.
    
    Para componentes físicos, a presença de imagens em docs/demos/
    é uma forte evidência de que o hardware está disponível.
    
    Args:
        component: Nome do componente
        
    Returns:
        Lista de imagens encontradas
    """
    demos_path = REPO_ROOT / "docs" / "demos"
    if not demos_path.exists():
        return []
    
    images = []
    for ext in ['*.jpg', '*.jpeg', '*.png', '*.gif']:
        images.extend(demos_path.rglob(ext))
    
    return images


def validate_hardware_components() -> Tuple[Dict[str, List[Path]], float]:
    """Valida todos os componentes de hardware.
    
    Returns:
        Tupla com (dicionário de evidências por componente, score)
    """
    evidence_map = {}
    validated_count = 0
    
    for component in HARDWARE_COMPONENTS:
        mentions = search_component_mentions(component)
        if mentions:
            evidence_map[component] = mentions
            validated_count += 1
        else:
            evidence_map[component] = []
    
    # Calcula score: % de componentes com evidências
    score = validated_count / len(HARDWARE_COMPONENTS) if HARDWARE_COMPONENTS else 0.0
    
    return evidence_map, score


def print_report(evidence_map: Dict[str, List[Path]], score: float):
    """Imprime relatório detalhado da validação."""
    print("=" * 70)
    print("VALIDATOR: Hardware Availability (ASSUMP-L0-1)")
    print("=" * 70)
    print()
    
    for component in HARDWARE_COMPONENTS:
        evidences = evidence_map.get(component, [])
        status = "✓ VALIDATED" if evidences else "✗ NO EVIDENCE"
        print(f"{status}: {component}")
        
        if evidences:
            print(f"  Found in {len(evidences)} file(s):")
            # Mostra até 3 arquivos como exemplo
            for filepath in evidences[:3]:
                rel_path = filepath.relative_to(REPO_ROOT)
                print(f"    - {rel_path}")
            if len(evidences) > 3:
                print(f"    ... and {len(evidences) - 3} more")
        print()
    
    # Verifica imagens
    images = check_image_evidence("hardware")
    if images:
        print(f"✓ Found {len(images)} demonstration image(s) in docs/demos/")
        print()
    
    print("-" * 70)
    print(f"SCORE: {score:.2f} ({int(score * 100)}% components validated)")
    print("-" * 70)
    
    if score == 1.0:
        print("STATUS: ✓ ALL COMPONENTS VALIDATED")
        print("All hardware components have documented evidence.")
    elif score >= 0.5:
        print("STATUS: ⚠ PARTIAL VALIDATION")
        print("Some components lack evidence. Consider adding documentation.")
    else:
        print("STATUS: ✗ INSUFFICIENT EVIDENCE")
        print("Most components lack evidence. Hardware availability not confirmed.")
    print()


def main():
    """Entry point do validator."""
    evidence_map, score = validate_hardware_components()
    print_report(evidence_map, score)
    
    # Exit code determina o comportamento do trudag:
    # 0 = sucesso total (score = 1.0)
    # 1 = sucesso parcial (score entre 0.0 e 1.0)
    # 2 = falha total (score = 0.0)
    
    if score == 1.0:
        sys.exit(0)
    elif score > 0.0:
        sys.exit(1)
    else:
        sys.exit(2)


if __name__ == '__main__':
    main()
