"""
Custom reference types for trudag.
This file provides WebReference for GitHub/GitLab URLs.
"""

import requests
from trudag.dotstop.core.reference.references import BaseReference


class WebReference(BaseReference):
    """
    Reference to a webpage URL (e.g., GitHub/GitLab).
    
    Usage in YAML frontmatter:
    ```yaml
    references:
    - type: webpage
      url: github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_Installation_Guide.md
    ```
    """
    
    def __init__(self, url: str) -> None:
        """
        Initialize WebReference with a URL.
        
        Args:
            url: URL to reference (without https:// prefix)
        """
        self._url = url
    
    @classmethod
    def type(cls) -> str:
        """Return the type identifier for YAML frontmatter."""
        return "webpage"
    
    @property
    def content(self) -> bytes:
        """
        Fetch webpage content for hashing.
        
        Returns:
            Raw HTML/text content as bytes
        """
        try:
            response = requests.get(f"https://{self._url}", timeout=10)
            response.raise_for_status()
            return response.text.encode()
        except Exception as e:
            # Return error message as content if fetch fails
            return f"Error fetching {self._url}: {str(e)}".encode()
    
    def as_markdown(self, filepath: None | str = None) -> str:
        """
        Return markdown representation of the reference.
        
        Args:
            filepath: Optional file path context (unused)
            
        Returns:
            Markdown link to the URL
        """
        return f"[{self._url}](https://{self._url})"
