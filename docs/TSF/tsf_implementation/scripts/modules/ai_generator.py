#!/usr/bin/env python3
"""
ai_generator.py - AI Content Generation Module for TSF Automation

Supports multiple AI methods:
- Option C: GitHub Copilot CLI (primary)
- Option G: Manual/VSCode (fallback)
- Future: Ollama, APIs
"""

import subprocess
import sys
from pathlib import Path
from typing import Optional, Dict, Any

class AIGenerator:
    """Handles AI content generation for TSF items."""

    def __init__(self, method: str = "copilot_cli"):
        self.method = method

    def generate_content(self, item_type: str, requirement_text: str, context: Dict[str, Any] = None) -> str:
        """
        Generate content for TSF item using selected AI method.

        Args:
            item_type: 'EXPECT', 'ASSERT', 'EVID', 'ASSUMP'
            requirement_text: The requirement to base generation on
            context: Additional context (ID, level, etc.)

        Returns:
            Generated content string
        """
        if self.method == "copilot_cli":
            return self._generate_with_copilot_cli(item_type, requirement_text, context)
        elif self.method == "manual":
            return self._generate_manual_placeholder(item_type, requirement_text, context)
        else:
            raise ValueError(f"Unsupported method: {self.method}")

    def _generate_with_copilot_cli(self, item_type: str, requirement_text: str, context: Dict[str, Any] = None) -> str:
        """Generate using GitHub Copilot CLI."""
        id_str = context.get('id', 'L0-X') if context else 'L0-X'

        prompts = {
            'EXPECT': f"Generate a TSF EXPECTATION item for requirement: {requirement_text}. Format as YAML frontmatter with id, header, text, level, normative, references, reviewers.",
            'ASSERT': f"Generate a TSF ASSERTION item for requirement: {requirement_text}. Format as YAML frontmatter with id, header, text, level, normative, references, reviewers.",
            'EVID': f"Generate a TSF EVIDENCE item for requirement: {requirement_text}. Format as YAML frontmatter with id, header, text, level, normative, references, evidence configuration.",
            'ASSUMP': f"Generate a TSF ASSUMPTION item for requirement: {requirement_text}. Format as YAML frontmatter with id, header, text, level, normative, references, reviewers."
        }

        prompt = prompts.get(item_type, f"Generate TSF {item_type} for: {requirement_text}")

        try:
            # Call gh copilot suggest
            result = subprocess.run(
                ["gh", "copilot", "suggest", "-t", "shell", prompt],
                capture_output=True,
                text=True,
                timeout=30
            )

            if result.returncode == 0:
                return result.stdout.strip()
            else:
                print(f"❌ Copilot CLI failed: {result.stderr}")
                return self._generate_manual_placeholder(item_type, requirement_text, context)

        except (subprocess.TimeoutExpired, FileNotFoundError) as e:
            print(f"⚠️  Copilot CLI not available ({e}), falling back to manual")
            return self._generate_manual_placeholder(item_type, requirement_text, context)

    def _generate_manual_placeholder(self, item_type: str, requirement_text: str, context: Dict[str, Any] = None) -> str:
        """Generate placeholder content for manual editing."""
        id_str = context.get('id', 'L0-X') if context else 'L0-X'

        templates = {
            'EXPECT': f"""---
id: EXPECT-{id_str}
header: {requirement_text[:50]}...
text: The system shall {requirement_text.lower()}.
level: 1.1
normative: true
references:
- id: ASSERT-{id_str}
  type: file
  path: ../assertions/ASSERT-{id_str}.md
- id: EVID-{id_str}
  type: file
  path: ../evidences/EVID-{id_str}.md
- id: ASSUMP-{id_str}
  type: file
  path: ../assumptions/ASSUMP-{id_str}.md
reviewers:
- name: João Silva
  email: joao.silva@seame.pt
---

The system shall {requirement_text.lower()} to ensure proper functionality.""",

            'ASSERT': f"""---
id: ASSERT-{id_str}
header: {requirement_text[:50]} is implemented
text: The {item_type.lower()} shall verify that {requirement_text.lower()}.
level: 1.1
normative: true
references:
- id: EXPECT-{id_str}
  type: file
  path: ../expectations/EXPECT-{id_str}.md
- id: EVID-{id_str}
  type: file
  path: ../evidences/EVID-{id_str}.md
- id: ASSUMP-{id_str}
  type: file
  path: ../assumptions/ASSUMP-{id_str}.md
reviewers:
- name: João Silva
  email: joao.silva@seame.pt
---

The implementation shall correctly {requirement_text.lower()}.""",

            'EVID': f"""---
id: EVID-{id_str}
header: {requirement_text[:50]} verification
text: Evidence demonstrates {requirement_text.lower()}.
level: 1.1
normative: true
references:
- id: EXPECT-{id_str}
  type: file
  path: ../expectations/EXPECT-{id_str}.md
- id: ASSERT-{id_str}
  type: file
  path: ../assertions/ASSERT-{id_str}.md
- id: ASSUMP-{id_str}
  type: file
  path: ../assumptions/ASSUMP-{id_str}.md
evidence:
  type: "test_execution"
  configuration:
    test: "validate_{id_str}"
---

Test logs and validation results confirm {requirement_text.lower()}.""",

            'ASSUMP': f"""---
id: ASSUMP-{id_str}
header: {requirement_text[:50]} environment
text: The system assumes {requirement_text.lower()[:50]}...
level: 1.0
normative: false
references:
- id: EXPECT-{id_str}
  type: file
  path: ../expectations/EXPECT-{id_str}.md
- id: ASSERT-{id_str}
  type: file
  path: ../assertions/ASSERT-{id_str}.md
- id: EVID-{id_str}
  type: file
  path: ../evidences/EVID-{id_str}.md
reviewers:
- name: João Silva
  email: joao.silva@seame.pt
---

The automation environment assumes Python 3.8+ and necessary dependencies for {requirement_text.lower()}."""
        }

        return templates.get(item_type, f"# PLACEHOLDER: {item_type}-{id_str}\n\nTODO: Generate content for {requirement_text}")


def main():
    """Test the AI generator."""
    if len(sys.argv) < 3:
        print("Usage: python ai_generator.py <method> <item_type> <requirement>")
        sys.exit(1)

    method = sys.argv[1]
    item_type = sys.argv[2]
    requirement = " ".join(sys.argv[3:])

    generator = AIGenerator(method)
    content = generator.generate_content(item_type, requirement, {'id': 'L0-TEST'})

    print("Generated content:")
    print("=" * 50)
    print(content)


if __name__ == "__main__":
    main()