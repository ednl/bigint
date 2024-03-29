#include <stdio.h>     // printf
#include <stdlib.h>    // malloc
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>   // bool
#include <string.h>    // memset
#ifdef __APPLE__
#include <pthread/qos.h>  // thread priority
#endif

// Number of values in one part of the bigint array = 1.10^WIDTH
#define UNIT  (10000000000000000ULL)

// Width in decimal digits of each part = log10(UNIT)
#define WIDTH "16"

// Number of parts to add when increasing the size of the bigint
#define CHUNK (4096ULL)

// The building blocks of the bigint value
typedef uint64_t PartType, *pPartType;

// BigInt type
typedef struct {
    PartType *part;
    size_t    len, maxlen;
} BigInt, *pBigInt;

// Free & reset
static void clean(pBigInt a)
{
    if (a->part != NULL) {
        free(a->part);
        a->part = NULL;
    }
    a->maxlen = 0;
    a->len = 0;
}

// Initialise with first chunk of memory
static bool init(pBigInt a)
{
    size_t memsize = CHUNK * sizeof(PartType);  // requested memory size in bytes
    void *p = a->part ? realloc(a->part, memsize) : malloc(memsize);
    if (p == NULL) {
        clean(a);
        return false;
    }
    memset(p, 0, memsize);
    a->part = (PartType *)p;
    a->maxlen = CHUNK;
    a->len = 0;
    return true;
}

// Load value into bigint
static bool setval(pBigInt a, uint64_t val)
{
    if (!init(a)) {
        return false;
    }
    // Distribute the value over successive parts
    while (val) {
        a->part[a->len++] = val % UNIT;
        val /= UNIT;
    }
    return true;
}

// Resize the bigint memory, clip to multiples of CHUNK
static bool resize(pBigInt a, size_t newlen)
{
    // Zero length = reset
    if (newlen == 0) {
        clean(a);
        return true;
    }
    size_t m = newlen % CHUNK;
    if (m) {
        newlen += CHUNK - m;
    }
    // Can't be smaller than value in it
    if (newlen < a->len) {
        return false;
    }
    if (newlen != a->maxlen) {
        size_t memsize = newlen * sizeof(PartType);
        void *p = a->part ? realloc(a->part, memsize) : malloc(memsize);
        if (p == NULL) {
            return false;
        }
        a->part = (PartType *)p;
        if (newlen > a->maxlen) {
            // Zero out the newly added memory
            // (here because pointer arithmatic with void pointers not part of the standard)
            memset(a->part + a->maxlen, 0, (newlen - a->maxlen) * sizeof(PartType));
        }
        a->maxlen = newlen;
    }
    return true;
}

// Add a + b, put result in c
static inline bool add(pBigInt a, pBigInt b, pBigInt c)
{
    size_t i = 0, minlen, maxlen, needed;
    BigInt *t;

    if (a->len < b->len) {
        minlen = a->len;
        maxlen = b->len;
        t = b;
    } else {
        minlen = b->len;
        maxlen = a->len;
        t = a;
    }

    needed = maxlen + 1;
    if (needed > c->maxlen && !resize(c, needed)) {
        return false;
    }

    PartType sum = 0;  // partial sum and carry
    while (i < minlen) {
        sum += a->part[i] + b->part[i];
        if (sum < UNIT) {
            c->part[i] = sum;
            sum = 0;  // no carry
        } else {
            c->part[i] = sum - UNIT;
            sum = 1;  // for addition, carry can only be 0 or 1
        }
        ++i;
    }
    while (i < maxlen) {
        if (sum) {
            sum += t->part[i];  // t = the one that's longer
            if (sum < UNIT) {
                c->part[i] = sum;
                sum = 0;  // no carry
            } else {
                // Only get here if a.len != b.len
                // and there is a carry from the minlen loop
                // and the value of the longer part is UNIT - 1
                // but it doesn't happen in the first 1000000 Fibonacci numbers (with WIDTH = 16)
                c->part[i] = sum - UNIT;
                sum = 1;  // for addition, carry can only be 0 or 1
            }
        } else {
            c->part[i] = t->part[i];
        }
        ++i;
    }
    if (sum) {
        c->part[i++] = sum;
    }
    c->len = i;
    return true;
}

// Print the bigint to stdout
static void print(pBigInt a)
{
    size_t i = a->len;
    if (i == 0) {
        printf("0 (0)\n");
        return;
    }
    printf("%"PRIu64, a->part[--i]);  // no leading zeros on left-most part
    // while (i--) {
    //     // Print in chunks of WIDTH digits
    //     printf(" %0"WIDTH""PRIu64, a->part[i]);
    // }
    // DEBUG: no full print out
    printf(" ... %0"WIDTH""PRIu64, a->part[0]);
    printf(" (%zu)\n", a->len);  // parts count for reference
}

int main(int argc, char *argv[])
{
    #ifdef __APPLE__
    // Set thread priority to QOS_CLASS_USER_INTERACTIVE = use a performance core
    //   to use efficiency core: QOS_CLASS_BACKGROUND
    int e = pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
    if (e) {
        printf("Pthread error: %d\n", e);
        exit(1);
    }
    #endif

    BigInt a = {0}, b = {0}, c = {0};
    setval(&a, 0);
    setval(&b, 1);
    setval(&c, 1);

    // Get requested Fibonacci ordinal from command line
    int n = 0;
    if (argc > 1) {
        n = atoi(argv[1]);
        if (n < 0) {
            n = 0;
        }
    }
    int m = n % 3;
    n /= 3;  // while-loop below is unrolled by 3

    while (n--) {
        add(&b, &c, &a);
        add(&c, &a, &b);
        add(&a, &b, &c);
    }
    switch (m) {
        case 0: print(&a); break;
        case 1: print(&b); break;
        case 2: print(&c); break;
    }

    clean(&a);
    clean(&b);
    clean(&c);
    return 0;
}
