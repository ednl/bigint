#include <stdio.h>     // printf
#include <stdlib.h>    // strtoull
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64

int main(int argc, char *argv[])
{
    uint64_t a = 0, b = 1, c = 1;  // Fibonacci numbers
    uint64_t q = 0, r = 0, s = 0;  // quotient, remainder, sum
    uint64_t N = 0;                // loop counter

    if (argc > 1) {
        N = strtoull(argv[1], NULL, 10);
    }

    while (N--) {
        a = b + c; q = a / c; r = a % c; s += q + r;
        b = c + a; q = b / a; r = b % a; s += q + r;
        c = a + b; q = c / b; r = c % b; s += q + r;
    }

    printf("%"PRIu64"\n", s);  // avoid optimising away div,mod
    return 0;
}
