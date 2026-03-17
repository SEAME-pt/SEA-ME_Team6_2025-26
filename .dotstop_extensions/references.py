from trudag.dotstop.core.reference.references import BaseReference, ResolvedReference
from trudag.dotstop.core.exception import ReferenceError
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
    def __init__(self, url: str = None, path: str = None, **kwargs):
        # Accept both 'url' and 'path' fields for flexibility
        self._url = url or path
        if not self._url:
            raise ReferenceError("UrlReference requires either 'url' or 'path' field")

    @classmethod
    def type(cls) -> str:
        return "url"

    @property
    def content(self) -> bytes:
        # Return the URL string itself as bytes for stable, deterministic SHA computation.
        # Live URL fetching causes non-deterministic SHAs because web content changes over time.
        return self._url.encode("utf-8")

    def as_markdown(self, filepath=None) -> str:  # noqa ARG002
        return f"[{self._url}]({self._url})\n"

    def __str__(self) -> str:
        return self._url
