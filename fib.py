#!/usr/bin/env python3

from sys import argv

try:
    N = int(argv[1])
except (IndexError, ValueError):
    exit("Not a number")

a = 0
b = 1
for _ in range(N):
    a, b = b, a + b

print(a)
