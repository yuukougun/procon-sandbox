#!/usr/bin/env python3
"""Build Doxygen/Sphinx documentation and assemble a GitHub Pages site."""

from __future__ import annotations

import argparse
import html
import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Any, Dict, Iterable, List


def run_command(command: List[str], cwd: Path | None = None) -> None:
    result = subprocess.run(command, cwd=cwd, text=True)
    if result.returncode != 0:
        joined = " ".join(command)
        raise RuntimeError(f"Command failed ({result.returncode}): {joined}")


def resolve_source_paths(repo_root: Path, source_paths: Iterable[str]) -> List[Path]:
    resolved: List[Path] = []
    for raw_path in source_paths:
        path = (repo_root / raw_path).resolve()
        if not path.exists():
            raise FileNotFoundError(f"Configured source path does not exist: {raw_path}")
        resolved.append(path)
    return resolved


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8")


def build_cpp_docs(library: Dict[str, Any], repo_root: Path, output_dir: Path) -> None:
    source_paths = resolve_source_paths(repo_root, library["source_paths"])

    with tempfile.TemporaryDirectory(prefix=f"doxygen-{library['id']}-") as tmp:
        temp_dir = Path(tmp)
        doxyfile_path = temp_dir / "Doxyfile"
        doxygen_output = temp_dir / "out"

        input_paths = " ".join(str(path) for path in source_paths)

        doxyfile = "\n".join(
            [
                f"PROJECT_NAME = \"{library['title']}\"",
                f"OUTPUT_DIRECTORY = {doxygen_output}",
                f"INPUT = {input_paths}",
                "RECURSIVE = YES",
                "FILE_PATTERNS = *.h *.hpp *.hh *.c *.cc *.cpp",
                "GENERATE_HTML = YES",
                "GENERATE_LATEX = NO",
                "EXTRACT_ALL = YES",
                "EXTRACT_PRIVATE = NO",
                "EXTRACT_STATIC = NO",
                "SOURCE_BROWSER = NO",
                "INLINE_SOURCES = NO",
                "REFERENCED_BY_RELATION = NO",
                "REFERENCES_RELATION = NO",
                "FULL_PATH_NAMES = NO",
                "QUIET = NO",
                "WARN_IF_UNDOCUMENTED = NO",
            ]
        )

        write_text(doxyfile_path, doxyfile + "\n")
        run_command(["doxygen", str(doxyfile_path)])

        html_dir = doxygen_output / "html"
        if not html_dir.exists():
            raise RuntimeError(f"Doxygen output was not created for library: {library['id']}")

        output_dir.mkdir(parents=True, exist_ok=True)
        shutil.copytree(html_dir, output_dir, dirs_exist_ok=True)


def infer_python_modules(source_paths: Iterable[Path]) -> List[str]:
    modules: List[str] = []
    for source_path in source_paths:
        if source_path.is_file() and source_path.suffix == ".py":
            modules.append(source_path.stem)
            continue

        if source_path.is_dir():
            for file_path in sorted(source_path.glob("*.py")):
                if file_path.name.startswith("_"):
                    continue
                modules.append(file_path.stem)

    deduped = sorted(set(modules))
    if not deduped:
        raise RuntimeError("No Python modules found. Set python_modules in docs/libraries.json.")
    return deduped


def build_python_docs(library: Dict[str, Any], repo_root: Path, output_dir: Path) -> None:
    source_paths = resolve_source_paths(repo_root, library["source_paths"])
    modules = library.get("python_modules") or infer_python_modules(source_paths)
    mock_imports = library.get("python_mock_imports", [])

    with tempfile.TemporaryDirectory(prefix=f"sphinx-{library['id']}-") as tmp:
        temp_dir = Path(tmp)
        source_dir = temp_dir / "source"
        build_dir = temp_dir / "build"

        sys_path_lines = [
            f"sys.path.insert(0, {str(path)!r})" for path in source_paths
        ]

        conf_py = "\n".join(
            [
                "import os",
                "import sys",
                "",
                *sys_path_lines,
                "",
                f"project = {library['title']!r}",
                "extensions = [",
                "    'sphinx.ext.autodoc',",
                "    'sphinx.ext.autosummary',",
                "    'sphinx.ext.napoleon',",
                "]",
                "autosummary_generate = True",
                "autodoc_default_options = {",
                "    'members': True,",
                "    'show-inheritance': True,",
                "}",
                f"autodoc_mock_imports = {mock_imports!r}",
                "html_theme = 'alabaster'",
                "html_show_sourcelink = False",
                "",
            ]
        )

        api_lines = [
            "API Reference",
            "=============",
            "",
            ".. autosummary::",
            "   :toctree: generated",
            "",
        ]
        api_lines.extend(f"   {module}" for module in modules)

        index_rst = "\n".join(
            [
                library["title"],
                "=" * len(library["title"]),
                "",
                library.get("description", ""),
                "",
                ".. toctree::",
                "   :maxdepth: 2",
                "",
                "   api",
                "",
            ]
        )

        write_text(source_dir / "conf.py", conf_py)
        write_text(source_dir / "api.rst", "\n".join(api_lines) + "\n")
        write_text(source_dir / "index.rst", index_rst)

        run_command(
            [
                "sphinx-build",
                "-b",
                "html",
                str(source_dir),
                str(build_dir),
                "-W",
                "--keep-going",
            ]
        )

        output_dir.mkdir(parents=True, exist_ok=True)
        shutil.copytree(build_dir, output_dir, dirs_exist_ok=True)


def build_homepage(entries: List[Dict[str, str]], output_dir: Path) -> None:
    cards = []
    for entry in entries:
        cards.append(
            "\n".join(
                [
                    '<li class="card">',
                    f"  <h2>{html.escape(entry['title'])}</h2>",
                    f"  <p class=\"meta\">{html.escape(entry['language'])}</p>",
                    f"  <p>{html.escape(entry['description'])}</p>",
                    f"  <a href=\"{html.escape(entry['url'])}\">Open documentation</a>",
                    "</li>",
                ]
            )
        )

    page = "\n".join(
        [
            "<!doctype html>",
            "<html lang=\"en\">",
            "<head>",
            "  <meta charset=\"utf-8\" />",
            "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />",
            "  <title>Library Documentation Hub</title>",
            "  <style>",
            "    :root {",
            "      --bg: #f5f7fb;",
            "      --card: #ffffff;",
            "      --text: #1a1f2e;",
            "      --sub: #56607a;",
            "      --accent: #1461ff;",
            "      --border: #d9e1f2;",
            "    }",
            "    body {",
            "      margin: 0;",
            "      background: radial-gradient(circle at top right, #dde7ff 0%, var(--bg) 45%);",
            "      color: var(--text);",
            "      font-family: 'Segoe UI', 'Hiragino Kaku Gothic ProN', sans-serif;",
            "    }",
            "    main {",
            "      max-width: 1000px;",
            "      margin: 0 auto;",
            "      padding: 40px 20px 56px;",
            "    }",
            "    h1 {",
            "      margin: 0 0 10px;",
            "      font-size: clamp(28px, 5vw, 44px);",
            "    }",
            "    .lead {",
            "      color: var(--sub);",
            "      margin: 0 0 24px;",
            "    }",
            "    ul {",
            "      list-style: none;",
            "      padding: 0;",
            "      margin: 0;",
            "      display: grid;",
            "      gap: 16px;",
            "      grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));",
            "    }",
            "    .card {",
            "      border: 1px solid var(--border);",
            "      border-radius: 14px;",
            "      background: var(--card);",
            "      padding: 18px;",
            "      box-shadow: 0 8px 24px rgba(20, 50, 120, 0.08);",
            "    }",
            "    .card h2 {",
            "      margin: 0 0 6px;",
            "      font-size: 20px;",
            "    }",
            "    .meta {",
            "      margin: 0 0 10px;",
            "      color: var(--sub);",
            "      text-transform: uppercase;",
            "      letter-spacing: 0.04em;",
            "      font-size: 12px;",
            "    }",
            "    .card a {",
            "      color: var(--accent);",
            "      font-weight: 600;",
            "      text-decoration: none;",
            "    }",
            "    .card a:hover {",
            "      text-decoration: underline;",
            "    }",
            "  </style>",
            "</head>",
            "<body>",
            "<main>",
            "  <h1>Library Documentation Hub</h1>",
            "  <p class=\"lead\">Auto-generated API documentation for C++ and Python libraries.</p>",
            "  <ul>",
            *cards,
            "  </ul>",
            "</main>",
            "</body>",
            "</html>",
            "",
        ]
    )

    write_text(output_dir / "index.html", page)


def validate_library(library: Dict[str, Any]) -> None:
    required_keys = ["id", "title", "language", "source_paths", "description", "output_subdir", "enabled"]
    for key in required_keys:
        if key not in library:
            raise KeyError(f"Library config is missing key '{key}': {library}")

    if library["language"] not in {"cpp", "python"}:
        raise ValueError(f"Unsupported language '{library['language']}' for library '{library['id']}'")

    if not isinstance(library["source_paths"], list) or not library["source_paths"]:
        raise ValueError(f"source_paths must be a non-empty list for library '{library['id']}'")


def build_site(config_path: Path, output_dir: Path) -> None:
    repo_root = Path(__file__).resolve().parents[2]
    config = json.loads(config_path.read_text(encoding="utf-8"))
    libraries = config.get("libraries", [])

    if output_dir.exists():
        shutil.rmtree(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    homepage_entries: List[Dict[str, str]] = []

    for library in libraries:
        validate_library(library)
        if not library["enabled"]:
            continue

        output_subdir = library["output_subdir"].strip("/")
        library_output = output_dir / output_subdir

        print(f"[docs] Building {library['id']} ({library['language']})")
        if library["language"] == "cpp":
            build_cpp_docs(library, repo_root, library_output)
        elif library["language"] == "python":
            build_python_docs(library, repo_root, library_output)

        homepage_entries.append(
            {
                "title": library["title"],
                "language": library["language"],
                "description": library.get("description", ""),
                "url": f"./{output_subdir}/index.html",
            }
        )

    build_homepage(homepage_entries, output_dir)
    write_text(output_dir / ".nojekyll", "")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Build docs site for GitHub Pages")
    parser.add_argument("--config", required=True, type=Path, help="Path to docs/libraries.json")
    parser.add_argument("--output", required=True, type=Path, help="Output directory for static site")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        build_site(args.config.resolve(), args.output.resolve())
    except Exception as error:  # noqa: BLE001
        print(f"[docs] ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
