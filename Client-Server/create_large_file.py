#! /usr/bin/python3

with open("1mb.txt", "w") as out:
    out.seek((1024 * 1024) -1)
    out.write('\0')