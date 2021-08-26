#include <stdio.h>     // printf
#include <stdlib.h>    // strtoull, rand
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64

#define LEN (1024U)
static uint64_t a[LEN] = {0}, b[LEN] = {0};

int main(int argc, char *argv[])
{
    uint64_t q = 0, r = 0, s = 0;  // quotient, remainder, sum
    uint64_t N = 0;                // loop counter

    if (argc > 1) {
        N = strtoull(argv[1], NULL, 10);
    }

    for (size_t i = 0; i < LEN; ++i) {
        int t1 = rand(), t2 = rand();
        if (t1 < t2) {
            a[i] = (uint64_t)t2;
            b[i] = (uint64_t)t1;
        } else {
            a[i] = (uint64_t)t1;
            b[i] = (uint64_t)t2;
        }
    }

    while (N--) {
        for (size_t i = 0; i < LEN; ++i) {
            q = a[i] / b[i];
            r = a[i] % b[i];
            s += q + r;
        }
    }

    printf("%"PRIu64"\n", s);  // avoid optimising away div,mod
    return 0;
}
