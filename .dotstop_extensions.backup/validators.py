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
