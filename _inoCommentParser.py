#!/usr/bin/env python3
"""
Complete Arduino Structure Comment Parser

A two-phase parser that:
1) Formats Arduino code using clang-format (plus indents else-if blocks)
2) Adds structure comments with focus on functions and control structures, correctly handling if-else chains

Usage: ./arduino_parser.py input.ino -o output.ino
"""

import subprocess
import sys
import os
import argparse
import tempfile
import re
from typing import List, Tuple, Optional, Set

# Only include the tags we want to use
STRUCTURE_TAGS = {
    "if": ("beginif", "endif"),
    "for": ("beginfor", "endfor"),
    "while": ("beginwhile", "endwhile"),
    "function": ("beginfunc", "endfunc"),
}

# PHASE 1: Code Formatting with clang-format + indent-else post-pass


def create_clang_config():
    """Create a temporary clang-format configuration file for Arduino code."""
    config = """
Language: Cpp
BasedOnStyle: Google
IndentWidth: 4
AccessModifierOffset: -4
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
AllowShortFunctionsOnASingleLine: None
BreakBeforeBraces: Allman
ColumnLimit: 100
PointerAlignment: Left
SortIncludes: false
ReflowComments: false
UseTab: Never
SpacesBeforeTrailingComments: 1
"""
    fd, path = tempfile.mkstemp(prefix=".clang-format-", text=True)
    with os.fdopen(fd, "w") as f:
        f.write(config)
    return path


def indent_else_blocks(code: str) -> str:
    """
    After formatting, indent 'else'/'else if' lines one level deeper
    under the preceding block's closing brace.
    """
    lines = code.split("\n")
    new_lines = []
    for line in lines:
        stripped = line.lstrip()
        if stripped.startswith("else") and new_lines:
            prev = new_lines[-1]
            prev_indent = len(prev) - len(prev.lstrip())
            new_indent = prev_indent + 4
            new_lines.append(" " * new_indent + stripped)
        else:
            new_lines.append(line)
    return "\n".join(new_lines)


def format_code(code: str) -> str:
    """
    Format Arduino code using clang-format, then indent else-if blocks.

    Args:
        code: The original code

    Returns:
        Formatted code with else-if indented
    """
    try:
        config_path = create_clang_config()
        try:
            subprocess.run(
                ["clang-format", "--version"],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=True,
            )
        except (subprocess.SubprocessError, FileNotFoundError):
            print(
                "Error: clang-format not found. Please install clang-format.",
                file=sys.stderr,
            )
            os.unlink(config_path)
            sys.exit(1)
        proc = subprocess.run(
            ["clang-format", f"-style=file:{config_path}"],
            input=code.encode("utf-8"),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True,
        )
        formatted = proc.stdout.decode("utf-8")
        os.unlink(config_path)
        return indent_else_blocks(formatted)
    except Exception as e:
        print(f"Error during formatting: {e}", file=sys.stderr)
        sys.exit(1)


# PHASE 2: Structure Comment Addition


def add_structure_comments(code: str) -> str:
    """
    Add structure comments focusing only on selected block types, handling if-else chains.

    Args:
        code: The formatted code

    Returns:
        Code with added structure comments
    """
    lines = code.split("\n")
    result = lines.copy()

    # Function regex: exclude control keywords at start
    function_regex = re.compile(
        r"^\s*(?!(?:if|for|while|else)\b)[\w\s\*\&\:\<\>\~]+\w+\s*\([^;{]*\)\s*$"
    )

    blocks: List[Tuple[str, int, Optional[int], Optional[int]]] = []
    brace_stack: List[Tuple[int, int, Optional[int]]] = []
    tagged_lines: Set[int] = set()

    # First pass: identify blocks and track braces
    for i, line in enumerate(lines):
        stripped = line.strip()
        if not stripped or stripped.startswith("//"):
            continue
        indent = len(line) - len(line.lstrip())
        # detect control
        if re.match(r"^\s*if\s*\(", stripped):
            blocks.append(("if", i, None, None))
        elif re.match(r"^\s*for\s*\(", stripped):
            blocks.append(("for", i, None, None))
        elif re.match(r"^\s*while\s*\(", stripped):
            blocks.append(("while", i, None, None))
        # detect function
        elif function_regex.match(stripped):
            blocks.append(("function", i, None, None))
        # opening brace
        if "{" in stripped:
            if blocks and blocks[-1][1] in (i, i - 1) and blocks[-1][2] is None:
                btype, sline, _, eline = blocks[-1]
                blocks[-1] = (btype, sline, i, eline)
                bidx = len(blocks) - 1
            else:
                bidx = None
            brace_stack.append((i, indent, bidx))
        # closing brace
        if "}" in stripped:
            if brace_stack:
                _, _, bidx = brace_stack.pop()
                if bidx is not None:
                    btype, sline, ob, _ = blocks[bidx]
                    blocks[bidx] = (btype, sline, ob, i)

    # Extend if-block end_lines to include else/else-if chains
    updated = []
    for btype, sline, ob, eline in blocks:
        if btype == "if" and eline is not None:
            chain_end = eline
            j = eline + 1
            while j < len(lines):
                st = lines[j].strip()
                if not st or st.startswith("//"):
                    j += 1
                    continue
                if st.startswith("else"):
                    # find where this else block closes
                    # locate '{'
                    if "{" in st:
                        open_j = j
                    else:
                        k = j + 1
                        while k < len(lines) and "{" not in lines[k]:
                            k += 1
                        open_j = k
                    # match braces
                    count = 0
                    for ch in lines[open_j]:
                        if ch == "{":
                            count += 1
                        if ch == "}":
                            count -= 1
                    m = open_j + 1
                    while m < len(lines) and count > 0:
                        for ch in lines[m]:
                            if ch == "{":
                                count += 1
                            if ch == "}":
                                count -= 1
                        m += 1
                    chain_end = m - 1
                    j = chain_end + 1
                    continue
                break
            updated.append((btype, sline, ob, chain_end))
        else:
            updated.append((btype, sline, ob, eline))
    blocks = updated

    # Second pass: inject tags
    for btype, sline, ob, eline in blocks:
        if btype in STRUCTURE_TAGS and sline is not None and eline is not None:
            start_tag, end_tag = STRUCTURE_TAGS[btype]
            if sline not in tagged_lines and "//" not in result[sline]:
                result[sline] = f"{result[sline]} //{start_tag}"
                tagged_lines.add(sline)
            if eline not in tagged_lines and "//" not in result[eline]:
                result[eline] = f"{result[eline]} //{end_tag}"
                tagged_lines.add(eline)

    return "\n".join(result)


def process_file(
    input_file: str, output_file: str = None, skip_format: bool = False
) -> None:
    try:
        code = open(input_file).read()
        formatted = code if skip_format else format_code(code)
        final = add_structure_comments(formatted)
        if output_file:
            open(output_file, "w").write(final)
            print(f"Output written to {output_file}")
        else:
            print(final)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


def main():
    p = argparse.ArgumentParser()
    p.add_argument("input_file")
    p.add_argument("-o", "--output")
    p.add_argument("--skip-format", action="store_true")
    args = p.parse_args()
    process_file(args.input_file, args.output, args.skip_format)


if __name__ == "__main__":
    main()
