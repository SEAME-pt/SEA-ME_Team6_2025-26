"""Dotstop extensions for trudag - references and validators.

This module exports all custom references and validators for trudag.
The trudag tool imports from 'localplugins', so we create a symlink:
    localplugins -> .dotstop_extensions

This allows trudag to find:
- FileReference, UrlReference (from references.py)
- validate_hardware_availability, validate_linux_environment, 
  validate_software_dependencies (from validators.py)
"""

# Export references
from .references import FileReference, UrlReference

# Export validators
from .validators import (
    validate_hardware_availability,
    validate_linux_environment,
    validate_software_dependencies,
)

__all__ = [
    # References
    "FileReference",
    "UrlReference",
    # Validators
    "validate_hardware_availability",
    "validate_linux_environment", 
    "validate_software_dependencies",
]
