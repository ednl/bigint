#include <stdio.h>     // printf
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64

#define LOOP (5000000000ULL)

int main(void)
{
    uint64_t i = LOOP, sum = 0, a = 0, b = 1, c, q, r;

    while (i--) {
        c = a;
        a = b;
        b += c;
        q = b / a;
        r = b % a;
        sum += q + r;  // avoid optimising away q and r
    }

    printf("%"PRIu64"\n", sum);
    return 0;
}
