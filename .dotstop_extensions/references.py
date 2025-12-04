from trudag.dotstop.core.reference.references import BaseReference, ResolvedReference
from trudag.dotstop.core.exception import ReferenceError
import urllib.request


class UrlReference(BaseReference):
    def __init__(self, path: str, **kwargs):
        self._url = path

    @classmethod
    def type(cls) -> str:
        return "url"

    @property
    def content(self) -> bytes:
        try:
            resp = urllib.request.urlopen(self._url)
            return resp.read()
        except Exception as e:
            raise ReferenceError(f"Could not fetch URL {self._url}: {e}") from e

    def as_markdown(self, filepath=None) -> str:  # noqa ARG002
        return f"[{self._url}]({self._url})\n"

    def __str__(self) -> str:
        return self._url
