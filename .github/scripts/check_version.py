"""Verify that a git tag matches the static versions in pyproject.toml and setup.py."""

import ast
import sys
import tomllib
from pathlib import Path


def setup_version(path: Path) -> str:
    tree = ast.parse(path.read_text(encoding="utf-8"))
    for node in tree.body:
        call = node.value if isinstance(node, ast.Expr) else node
        if isinstance(call, ast.Call) and getattr(call.func, "id", None) == "setup":
            for keyword in call.keywords:
                if keyword.arg == "version" and isinstance(keyword.value, ast.Constant):
                    value = keyword.value.value
                    if isinstance(value, str):
                        return value
    raise SystemExit(f"version not found in {path}")


def main() -> None:
    tag = sys.argv[1]
    if not tag.startswith("v"):
        raise SystemExit(f"unexpected tag format: {tag}")
    expected = tag[1:]

    pyproject = tomllib.loads(Path("pyproject.toml").read_text(encoding="utf-8"))
    versions = {
        "pyproject.toml": pyproject["project"]["version"],
        "setup.py": setup_version(Path("setup.py")),
    }

    for name, version in versions.items():
        if version != expected:
            raise SystemExit(f"version mismatch: tag={tag}, {name}={version}")
    print(
        f"ok: tag={tag}, "
        + ", ".join(f"{name}={version}" for name, version in versions.items())
    )


if __name__ == "__main__":
    main()
