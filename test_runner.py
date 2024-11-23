#!/bin/env python3

# TODO: change to test program properly
import shlex
import sys
import subprocess

command, *arg = shlex.split("".join(sys.argv[1:]))
arg = "".join(arg)
output = "".join(c for c in arg if not c.isnumeric()).replace("+", "++").replace("-", "--")
output = bytes(f"{output}\n{output}\n", encoding="UTF-8")
result = subprocess.run([command, arg], capture_output=True)
if result.stdout != output:
    print(
        "Expected output:",
        output,
        "Test has not passed. Program output:",
        result.stdout,
        "Error log:",
        result.stderr,
        sep="\n",
    )
    sys.exit(1)
sys.exit(0)
