#!/usr/bin/env python3

from sys import argv

N = 0
if len(argv) > 1:
    N = int(argv[1])
    if N < 0:
        N = -N

a = 0
b = 1
for _ in range(N):
    a, b = b, a + b

print(a)
