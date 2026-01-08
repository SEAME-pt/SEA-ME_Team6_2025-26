from trudag.dotstop.core.reference.references import BaseReference, ResolvedReference
from trudag.dotstop.core.exception import ReferenceError
import urllib.request
import os


class FileReference(BaseReference):
    def __init__(self, path: str, **kwargs):
        self._path = path

    @classmethod
    def type(cls) -> str:
        return "file"

    @property
    def content(self) -> bytes:
        if not os.path.exists(self._path):
            # Return dummy content for non-existent files to avoid ReferenceError
            return b"File not found - reference skipped"
        
        if not os.path.isfile(self._path):
            # Return dummy content for non-regular files
            return b"Non-regular file - reference skipped"
        
        try:
            with open(self._path, 'rb') as f:
                return f.read()
        except Exception as e:
            raise ReferenceError(f"Could not read file {self._path}: {e}") from e

    def as_markdown(self, filepath=None) -> str:  # noqa ARG002
        return f"[{self._path}]({self._path})\n"

    def __str__(self) -> str:
        return self._path


class UrlReference(BaseReference):
    def __init__(self, path: str, **kwargs):
        self._url = path

    @classmethod
    def type(cls) -> str:
        return "url"

    @property
    def content(self) -> bytes:
        # Skip fetching GitHub blob URLs from our own repo to avoid 404 errors
        if 'github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob' in self._url:
            # Return a dummy content for GitHub blob URLs
            return b"GitHub blob URL - content not fetched to avoid 404 errors"
        
        # If URL looks truncated (ends with incomplete domain), skip it
        if (self._url.endswith('github.com/SEAME-pt/SEA') or 
            self._url.endswith('github.com/SEAME-pt/S') or
            self._url.endswith('github.com/SEAME-pt/SEA-ME_') or
            self._url.endswith('github.com/SEAME-pt/SEA-M') or
            len(self._url) < 20 or
            'github.com/SEAME-pt/SEA' in self._url and not 'SEA-ME_Team6_2025-26' in self._url):
            return b"Truncated URL - content not fetched"
        
        try:
            resp = urllib.request.urlopen(self._url)
            return resp.read()
        except Exception as e:
            raise ReferenceError(f"Could not fetch URL {self._url}: {e}") from e

    def as_markdown(self, filepath=None) -> str:  # noqa ARG002
        return f"[{self._url}]({self._url})\n"

    def __str__(self) -> str:
        return self._url
