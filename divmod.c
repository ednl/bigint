#include <stdio.h>     // printf
#include <stdlib.h>    // strtoull
#include <stdint.h>    // uint64_t, uint8_t
#include <inttypes.h>  // PRIu64

int main(int argc, char *argv[])
{
    uint64_t a = 0, b = 1, c = 1;  // Fibonacci numbers
    uint64_t q = 0, r = 0, s = 0;  // quotient, remainder, sum
    uint64_t N = 0;                // repeat counter
    uint8_t  f = 0;                // which Fib number to update

    if (argc > 1) {
        N = strtoull(argv[1], NULL, 10);
    }

    while (N--) {
        switch (f) {
            case 0: a = b + c; q = a / c; r = a % c; f = 1; break;
            case 1: b = c + a; q = b / a; r = b % a; f = 2; break;
            case 2: c = a + b; q = c / b; r = c % b; f = 0; break;
        }
        s += q + r;  // avoid optimising away q and r
    }

    printf("%"PRIu64"\n", s);
    return 0;
}
