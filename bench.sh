#!/bin/sh

# Number of iterations the tested program will use
#   REP = 1 billion will run in approx 0.7 s on a Mac mini M1
REP=1000000000

# Source code
SRC='divmod.c'
[ -f "$SRC" ] && [ -r "$SRC" ] || { echo "File not found: $SRC" >&2; exit 1; }

# Timing utility (install from MacPorts or Linux repo)
TIM=hyperfine
[ -z "$(which $TIM)" ] && { echo "App not found: $TIM" >&2; exit 2; }

# Test these C compilers
C1=clang
C2=clang-mp-12
C3=gcc
C4=gcc-mp-11

# Machine info
echo
uname -a
echo

# Compile and run
for CC in $C1 $C2 $C3 $C4; do
	if [ -n "$(which $CC)" ]; then
		echo "---------- $CC ----------"
		$CC --version
		$CC -Ofast "$SRC"
		ls -l a.out
		./a.out $REP
	 	$TIM -w 3 -r 10 "./a.out $REP"
	fi
done
