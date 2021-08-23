#include <stdio.h>     // printf
#include <stdlib.h>    // rand
#include <time.h>      // clock_gettime
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>   // bool, true, false

#define LOOP (2000000UL)
#define N    (1000UL)
static uint64_t a[N] = {0}, b[N] = {0};

static double timer(void)
{
    static bool start = true;
    static struct timespec t0;
    struct timespec t1;

    if (start) {
        start = false;
        clock_gettime(CLOCK_MONOTONIC_RAW, &t0);
        return 0;
    } else {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
        start = true;
        return 1.0 * t1.tv_sec + 1e-9 * t1.tv_nsec - (1.0 * t0.tv_sec + 1e-9 * t0.tv_nsec);
    }
}

int main(void)
{
    for (size_t i = 0; i < N; ++i) {
        int p = rand();
        int q = rand();
        if (p < q) {
            int r = p;
            p = q;
            q = r;
        }
        a[i] = (uint64_t)p;
        b[i] = (uint64_t)q;
    }
    uint64_t sum = 0;

    timer();
    for (size_t j = 0; j < LOOP; ++j) {
        for (size_t i = 0; i < N; ++i) {
            uint64_t q = a[i] / b[i];
            uint64_t r = a[i] % b[i];
            sum += q + r;
        }
    }
    double t = timer();

    FILE *f = fopen("divmod.txt", "a");
    fprintf(f, "%zu,%zu,%"PRIu64",%.5f\n", LOOP, N, sum, t);
    fclose(f);

    return 0;
}
