#!/usr/bin/env python3
"""
Arduino Code Formatter using clang-format

This script formats Arduino/C++ code using clang-format with Arduino-friendly settings.
"""

import subprocess
import sys
import os
import argparse
import tempfile

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
    fd, path = tempfile.mkstemp(prefix='.clang-format-', text=True)
    with os.fdopen(fd, 'w') as f:
        f.write(config)
    return path

def format_code(input_file, output_file=None):
    """
    Format Arduino code using clang-format.
    
    Args:
        input_file: Path to the input file
        output_file: Path to the output file (or None for stdout)
    """
    try:
        # Create a temporary clang-format config file
        config_path = create_clang_config()
        
        # Check if clang-format is available
        try:
            subprocess.run(['clang-format', '--version'], 
                          stdout=subprocess.PIPE, 
                          stderr=subprocess.PIPE, 
                          check=True)
        except (subprocess.SubprocessError, FileNotFoundError):
            print("Error: clang-format not found. Please install clang-format.", file=sys.stderr)
            os.unlink(config_path)
            sys.exit(1)
        
        # Read the input file
        with open(input_file, 'r') as f:
            code = f.read()
        
        # Run clang-format
        proc = subprocess.run(
            ['clang-format', f'-style=file:{config_path}'],
            input=code.encode('utf-8'),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True
        )
        
        formatted_code = proc.stdout.decode('utf-8')
        
        # Clean up the temp config file
        os.unlink(config_path)
        
        # Output the formatted code
        if output_file:
            with open(output_file, 'w') as f:
                f.write(formatted_code)
            print(f"Formatting complete. Output written to {output_file}")
        else:
            print(formatted_code)
            
    except FileNotFoundError:
        print(f"Error: Input file '{input_file}' not found.", file=sys.stderr)
        sys.exit(1)
    except subprocess.SubprocessError as e:
        print(f"Error running clang-format: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        sys.exit(1)

def main():
    """Parse command line arguments and run the formatter."""
    parser = argparse.ArgumentParser(description='Format Arduino code using clang-format')
    parser.add_argument('input_file', help='Input Arduino code file')
    parser.add_argument('-o', '--output', help='Output file (defaults to stdout)')
    
    args = parser.parse_args()
    format_code(args.input_file, args.output)

if __name__ == "__main__":
    main()