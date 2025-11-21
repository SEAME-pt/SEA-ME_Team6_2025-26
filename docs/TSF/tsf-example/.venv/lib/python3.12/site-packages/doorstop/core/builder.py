# SPDX-License-Identifier: LGPL-3.0-only

"""Functions to build a tree and access documents and items."""


import os
from collections.abc import Mapping
from pathlib import Path
from traceback import format_exception

from dulwich import porcelain
from dulwich.repo import Repo

from doorstop import common
from doorstop.common import DoorstopError
from doorstop.core import vcs
from doorstop.core.document import Document
from doorstop.core.tree import Tree

log = common.logger(__name__)
_tree: Tree | None = None  # implicit tree for convenience functions


def build(cwd=None, root=None, request_next_number=None) -> Tree:
    """Build a tree from the current working directory or explicit root.

    :param cwd: current working directory
    :param root: path to root of the working copy
    :param request_next_number: server method to get a document's next number

    :raises: :class:`~doorstop.common.DoorstopError` when the tree
        cannot be built

    :return: new :class:`~doorstop.core.tree.Tree`

    """

    documents: list[Document] = []

    # Find the root of the working copy
    cwd = cwd or os.getcwd()
    root = root or vcs.find_root(cwd)

    # external parents inclusion
    external_documents: dict[str, str] = {}  # Prefix, tag
    external_base_path: Path | None = (
        Path(root, ".doorstop-external") or None
    )  # TODO: make this configurable

    # Find all documents in the working copy
    log.info("looking for documents in {}...".format(root))
    skip_file_name = ".doorstop.skip-all"
    if not os.path.isfile(os.path.join(root, skip_file_name)):
        _document_from_path(
            root, root, documents, external_documents, external_base_path
        )
    exclude_dirnames = {".git", ".venv", "venv"}
    if not os.path.isfile(os.path.join(root, skip_file_name)):
        for dirpath, dirnames, _ in os.walk(root, topdown=True):
            whilelist_dirnames = []
            for dirname in dirnames:
                if dirname in exclude_dirnames:
                    continue
                path = os.path.join(dirpath, dirname)
                if os.path.isfile(os.path.join(path, skip_file_name)):
                    continue
                whilelist_dirnames.append(dirname)
                _document_from_path(
                    path, root, documents, external_documents, external_base_path
                )
            dirnames[:] = whilelist_dirnames

    # Build the tree
    if not documents:
        log.info("no documents found in: {}".format(root))

    for document in documents:
        log.info("found document: {}".format(document))

    log.info("building tree...")
    tree = Tree.from_list(documents, root=root)
    tree.request_next_number = request_next_number
    if len(tree):  # pylint: disable=len-as-condition
        log.info("built tree: {}".format(tree))
    else:
        log.info("tree is empty")
    return tree


def _check_for_duplicates(documents: list[Document], document: Document):
    if not any(docs.prefix == document.prefix for docs in documents):
        documents.append(document)


def _document_from_path(
    path,
    root,
    documents: list[Document],
    external_documents: dict[str, str],
    external_base_path: Path | None,
):
    """Attempt to create and append a document from the specified path.

    :param path: path to a potential document
    :param root: path to root of working copy
    :param documents: list of :class:`~doorstop.core.document.Document`
        to append results
    :param external_documents:  dictionary to control the version of external documents
    :param external_base_path: where external documents will be stored
    """
    try:
        document = Document(path, root, tree=None)  # tree attached later
    except DoorstopError:
        pass  # no document in directory
    else:
        if document.skip:
            log.debug("skipped document: {}".format(document))
        else:
            document.load()  # force to load the properties Earlier
            if (
                document.external_parent and document.external_parent_tag
            ) and external_base_path:
                if not getattr(document, "parent"):
                    raise DoorstopError(
                        "Document {} @ {} have external parent configs and it doesn't have a parent prefix".format(
                            document.prefix, document.path
                        )
                    )
                log.debug(
                    """Document has external parent
external information:
    external Parent: {}
    external Tag: {}""".format(
                        document.external_parent, document.external_parent_tag
                    )
                )

                _download_external_parent(
                    external_base_path,
                    document.external_parent,
                    document.external_parent_tag,
                    documents,
                    external_documents,
                )
            _check_for_duplicates(documents, document)


def find_document(prefix):
    """Find a document without an explicitly building a tree."""
    tree = _get_tree()
    document = tree.find_document(prefix)
    return document


def find_item(uid):
    """Find an item without an explicitly building a tree."""
    tree = _get_tree()
    item = tree.find_item(uid)
    return item


def _get_tree(request_next_number=None):
    """Get a shared tree for convenience functions."""
    global _tree
    if _tree is None:
        _tree = build()
    _tree.request_next_number = request_next_number
    return _tree


def _set_tree(value):
    """Set the shared tree to a specific value (for testing)."""
    global _tree
    _tree = value


def _clear_tree():
    """Force the shared tree to be rebuilt."""
    global _tree
    _tree = None


def _create_external_dir(external_base_path: Path):
    """Create the folder that will store pulled documents."""
    match external_base_path.exists():
        case True:
            log.debug(
                "Skipping creation folder already exists {} in {}.".format(
                    external_base_path.parts[-1], external_base_path.parent
                )
            )
        case False:
            log.info(
                "Creating folder {} in {}".format(
                    external_base_path.parts[-1], external_base_path.parent
                )
            )
    try:
        external_base_path.mkdir(exist_ok=True)
    except Exception as e:
        tb_str = "".join(format_exception(None, e, e.__traceback__))
        raise DoorstopError(
            """Something unexpected has happened when trying to create folder {} in {}\n.
                            Details: {}""".format(
                external_base_path.parent, external_base_path.parts[-1], tb_str
            )
        )


def _external_documents_check(
    existing_external_docs: Mapping[str, str], new_external_docs: Mapping[str, str]
):
    results: dict[str, bool] = {}

    for key in new_external_docs.keys():
        if key in existing_external_docs:
            results[key] = existing_external_docs[key] == new_external_docs[key]

    if list(results.values()).count(False) >= 1:
        raise DoorstopError(
            f"""
Different version for a prefix detected please check the external document definitions.
Affected prefix(es) { [ x for x,v in results.items() if v is False ] }
                             """
        )


def _validate_external_documents(
    new_external_doc: Path,
    external_document_tag: str,
    external_documents: dict[str, str],
    doorstop_documents: list[Document],
):
    """
    Reponsible to discover external documents and add them to the doorstop main document list.

    :param new_external_doc: path to a potential external document
    :param external_document_tag: tag to track which version of the document we are using
    :param external_documents:  dictionary to control the version of external documents format is Document Prefix: Tag (version value)
    :param doorstop_documents: list of :class:`~doorstop.core.document.Document` to append results
    """

    def find_external_docs(external_path: Path):
        """Reproduction partial builder's logic here for external docs."""
        root = new_external_doc.absolute().parent
        external_docs: list[Document] = []
        skip_file_name = ".doorstop.skip-all"
        exclude_dirnames = {".git", ".venv", "venv"}
        # case git root is a document
        if not os.path.isfile(os.path.join(root, skip_file_name)):
            _document_from_path(
                external_path, external_path, external_docs, external_documents, root
            )

        # search through folders
        for dirpath, dirnames, _ in os.walk(external_path, topdown=True):
            for dirname in dirnames:
                if dirname in exclude_dirnames:
                    continue
                path = os.path.join(dirpath, dirname)
                if os.path.isfile(os.path.join(path, skip_file_name)):
                    continue
                _document_from_path(
                    path, path, external_docs, external_documents, external_path
                )
        return external_docs

    new_external_docs = find_external_docs(new_external_doc)
    entries = {str(d.prefix): external_document_tag for d in new_external_docs}
    _external_documents_check(external_documents, entries)
    # external_documents.update({k:v for k,v in entries.items() if k not in external_documents.keys()})  #TODO: is it really necessary to filter?
    external_documents.update(entries)  # for now lets just throw the whole dict

    for nrd in new_external_docs:
        _check_for_duplicates(doorstop_documents, nrd)


def _git_pull(git_url: str, git_tag: str, target_folder: Path):

    def exact_want(refs, _depth=None):
        tag = (
            b"refs/heads/" + git_tag.encode()
            if git_tag in ["main", "master"]
            else b"refs/tags/" + git_tag.encode()
        )
        if tag in refs:
            return tag

        raise DoorstopError("ref {} not found in external {}".format(git_tag, git_url))

    path = str(target_folder.absolute())
    with open(os.devnull, "wb") as f:
        if not (target_folder / ".git").exists():
            Repo.init(path, mkdir=False)
            porcelain.pull(
                path,
                git_url,
                refspecs=exact_want(
                    porcelain.fetch(path, git_url, errstream=f, outstream=f)
                ),
                errstream=f,
                outstream=f,
                force=True,
                report_activity=None,
            )


def _download_external_parent(
    external_base_path: Path,
    external_parent_url: str,
    external_parent_tag: str,
    doorstop_documents: list[Document],
    external_documents: dict[str, str],
):

    _create_external_dir(external_base_path)

    folder_name = external_parent_url.split("/")[-1].split(".")[0]

    if not folder_name:
        raise DoorstopError(
            "Couldn't guess a name to storage external parent, check the config for {}".format(
                external_parent_url
            )
        )

    try:
        target = external_base_path / folder_name
        _create_external_dir(target)
        _git_pull(external_parent_url, external_parent_tag, target)
        _validate_external_documents(
            target, external_parent_tag, external_documents, doorstop_documents
        )
    except Exception as e:
        tb_str = "".join(format_exception(None, e, e.__traceback__))
        raise DoorstopError(
            """Unexpected error when downloading external parent from {}
ERROR Details: 
    {} """.format(
                external_parent_url, tb_str
            )
        )
