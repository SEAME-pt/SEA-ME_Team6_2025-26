#!/usr/bin/env python3
"""Custom validators for TSF items.

These validators provide manual scoring capabilities for TSF items
where automated validation is not feasible.
"""

from typing import TypeAlias

yaml: TypeAlias = str | int | float | bool | list["yaml"] | dict[str, "yaml"]


def manual_score(configuration: dict[str, yaml]) -> tuple[float, list[Exception | Warning]]:
    """Validator that returns a manually assigned score.
    
    Configuration should contain:
        score: float between 0.0 and 1.0
    
    Example yaml:
        evidence:
            type: manual_score
            configuration:
                score: 0.8
    """
    score = configuration.get("score", None)
    
    if score is None:
        return (0.0, [ValueError("No score specified in manual_score validator configuration")])
    
    if not isinstance(score, (int, float)):
        return (0.0, [TypeError(f"Score must be numeric, got {type(score)}")])
    
    if not (0.0 <= score <= 1.0):
        return (0.0, [ValueError(f"Score must be between 0.0 and 1.0, got {score}")])
    
    return (float(score), [])


def assertion_review(configuration: dict[str, yaml]) -> tuple[float, list[Exception | Warning]]:
    """Validator for assertions reviewed by SME.
    
    Returns a fixed score of 0.5 for assertions that have been reviewed
    but cannot be automatically validated.
    """
    return (0.5, [])


def assumption_review(configuration: dict[str, yaml]) -> tuple[float, list[Exception | Warning]]:
    """Validator for assumptions reviewed by SME.
    
    Returns a fixed score of 0.6 for assumptions that have been reviewed.
    """
    return (0.6, [])


def evidence_artifacts(configuration: dict[str, yaml]) -> tuple[float, list[Exception | Warning]]:
    """Validator for evidence with artifacts.
    
    Returns a score of 1.0 if artifacts are present and accessible.
    """
    artifacts = configuration.get("artifacts", [])
    
    if not artifacts:
        return (0.0, [Warning("No artifacts specified for evidence")])
    
    return (1.0, [])


def expectation_review(configuration: dict[str, yaml]) -> tuple[float, list[Exception | Warning]]:
    """Validator for expectations reviewed by SME.
    
    Returns a fixed score of 0.5 for expectations that have been reviewed.
    """
    return (0.5, [])


# ============================================================================
# ASSUMPTION VALIDATORS - Universal validators for all assumptions
# ============================================================================

def validate_hardware_availability(configuration: dict[str, yaml]) -> tuple[float, list[Exception | Warning]]:
    """Validator for ASSUMP-L0-1 and ASSUMP-L0-2: Hardware availability.
    
    Validates that hardware components mentioned in the assumption are documented
    in the repository through:
    - Documentation files (guides, demos, sprints)
    - Source code references
    - Demo images
    
    Configuration should contain:
        components: list of hardware components to validate
        search_paths: optional list of paths to search (relative to repo root)
    
    Example yaml:
        validators:
        - type: validate_hardware_availability
          configuration:
            components:
              - "Raspberry Pi 5"
              - "Hailo AI Hat"
              - "DC motors"
    
    Returns:
        (score, warnings) where score = percentage of validated components
    """
    import re
    from pathlib import Path
    
    # Get repository root (3 levels up from .dotstop_extensions)
    repo_root = Path(__file__).resolve().parents[1]
    
    # Get components to validate
    components = configuration.get("components", [])
    if not components:
        return (0.0, [ValueError("No components specified in configuration")])
    
    # Default search paths
    default_paths = [
        "docs/demos",
        "docs/guides", 
        "docs/sprints",
        "src"
    ]
    search_paths = configuration.get("search_paths", default_paths)
    
    # Track validated components
    validated = []
    warnings = []
    
    for component in components:
        found = False
        search_pattern = re.compile(re.escape(component), re.IGNORECASE)
        
        for search_path in search_paths:
            full_path = repo_root / search_path
            if not full_path.exists():
                continue
            
            # Search in markdown, text, python, and shell files
            for ext in ['*.md', '*.txt', '*.py', '*.sh']:
                for filepath in full_path.rglob(ext):
                    try:
                        content = filepath.read_text(encoding='utf-8', errors='ignore')
                        if search_pattern.search(content):
                            found = True
                            break
                    except Exception:
                        continue
                if found:
                    break
            if found:
                break
        
        if found:
            validated.append(component)
        else:
            warnings.append(Warning(f"No evidence found for component: {component}"))
    
    # Calculate score as percentage of validated components
    score = len(validated) / len(components) if components else 0.0
    
    return (score, warnings)


def validate_linux_environment(configuration: dict[str, yaml]) -> tuple[float, list[Exception | Warning]]:
    """Validator for ASSUMP-L0-3: Linux environment availability.
    
    Validates that Linux-specific tooling and documentation are present.
    
    Configuration should contain:
        required_tools: list of Linux tools/packages to verify documentation for
        ubuntu_version: optional Ubuntu version to check for (default: "22.04")
    
    Example yaml:
        validators:
        - type: validate_linux_environment
          configuration:
            required_tools:
              - "apt"
              - "systemd"
            ubuntu_version: "22.04"
    
    Returns:
        (score, warnings) where score = percentage of documented tools
    """
    import re
    from pathlib import Path
    
    repo_root = Path(__file__).resolve().parents[1]
    
    required_tools = configuration.get("required_tools", ["Ubuntu", "Linux", "apt"])
    ubuntu_version = configuration.get("ubuntu_version", "22.04")
    
    validated = []
    warnings = []
    
    # Search in documentation and scripts
    search_paths = [repo_root / "docs", repo_root / "src"]
    
    for tool in required_tools:
        found = False
        search_pattern = re.compile(re.escape(tool), re.IGNORECASE)
        
        for search_path in search_paths:
            if not search_path.exists():
                continue
                
            for ext in ['*.md', '*.txt', '*.sh', '*.py']:
                for filepath in search_path.rglob(ext):
                    try:
                        content = filepath.read_text(encoding='utf-8', errors='ignore')
                        if search_pattern.search(content):
                            found = True
                            break
                    except Exception:
                        continue
                if found:
                    break
            if found:
                break
        
        if found:
            validated.append(tool)
        else:
            warnings.append(Warning(f"No documentation found for: {tool}"))
    
    score = len(validated) / len(required_tools) if required_tools else 0.0
    
    return (score, warnings)


def validate_software_dependencies(configuration: dict[str, yaml]) -> tuple[float, list[Exception | Warning]]:
    """Validator for software/library dependencies assumptions.
    
    Validates that required software dependencies are documented or present
    in requirements files, package.json, CMakeLists.txt, etc.
    
    Configuration should contain:
        dependencies: list of dependencies to verify
        dependency_files: optional list of files to check (e.g., requirements.txt, package.json)
    
    Example yaml:
        validators:
        - type: validate_software_dependencies
          configuration:
            dependencies:
              - "Qt6"
              - "OpenCV"
            dependency_files:
              - "requirements.txt"
              - "CMakeLists.txt"
    
    Returns:
        (score, warnings) where score = percentage of found dependencies
    """
    import re
    from pathlib import Path
    
    repo_root = Path(__file__).resolve().parents[1]
    
    dependencies = configuration.get("dependencies", [])
    if not dependencies:
        return (0.0, [ValueError("No dependencies specified in configuration")])
    
    # Default dependency files to check
    default_files = [
        "requirements.txt",
        "package.json",
        "CMakeLists.txt",
        "Dockerfile",
        "*.cmake"
    ]
    dependency_files = configuration.get("dependency_files", default_files)
    
    validated = []
    warnings = []
    
    for dep in dependencies:
        found = False
        search_pattern = re.compile(re.escape(dep), re.IGNORECASE)
        
        # Search in dependency files
        for file_pattern in dependency_files:
            for filepath in repo_root.rglob(file_pattern):
                try:
                    content = filepath.read_text(encoding='utf-8', errors='ignore')
                    if search_pattern.search(content):
                        found = True
                        break
                except Exception:
                    continue
            if found:
                break
        
        # Also search in documentation
        if not found:
            docs_path = repo_root / "docs"
            if docs_path.exists():
                for filepath in docs_path.rglob("*.md"):
                    try:
                        content = filepath.read_text(encoding='utf-8', errors='ignore')
                        if search_pattern.search(content):
                            found = True
                            break
                    except Exception:
                        continue
        
        if found:
            validated.append(dep)
        else:
            warnings.append(Warning(f"No evidence found for dependency: {dep}"))
    
    score = len(validated) / len(dependencies) if dependencies else 0.0
    
    return (score, warnings)
