#!/usr/bin/python
# Split multi-game SGF file produced by twogtp.py into single-game files.

from re import search
from sys import stdin


def write_file():
    with open(filename, "a") as f:
        f.write(buffer)

filename = ""
buffer = ""
for line in stdin.readlines():
    match = search("GN\[(\d+)\]", line)
    if match:
        if filename != "":
            write_file()
        filename = match.group(1) + ".blksgf"
        buffer = ""
    buffer += line
write_file()
