#!/bin/sh

# Number of iterations the tested program will run
REP=200000000

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
		# Compile
		$CC -Ofast "$SRC"
		# Show binary file size
		ls -l a.out
		# Check program output
		./a.out $REP
		# Benchmark
	 	$TIM -w 3 -r 10 "./a.out $REP"
	fi
done
