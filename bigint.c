#include <stdio.h>    // printf
#include <stdlib.h>   // malloc
#include <stdint.h>   // uint32_t
#include <stdbool.h>  // bool
#include <string.h>   // memset

// Maximum value of each part of the bigint array
#define UNIT  (100000000U)

// Width in digits of each part
#define WIDTH "8"

// Number of parts to add when increasing the size of the bigint
#define CHUNK (1024U)

// The building blocks of the bigint value
typedef uint32_t PartType, *pPartType;

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
    a->part = (PartType *)p;
    a->maxlen = CHUNK;
    a->len = 0;
    memset(a->part, 0, memsize);
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
    unsigned int m = newlen % CHUNK;
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
            memset(a->part + a->maxlen, 0, (newlen - a->maxlen) * sizeof(PartType));
        }
        a->maxlen = newlen;
    }
    return true;
}

// Add a + b, put result in c
static bool add(pBigInt a, pBigInt b, pBigInt c)
{
    size_t maxlen = (a->len > b->len ? a->len : b->len) + 1;
    if (maxlen > c->maxlen && !resize(c, maxlen)) {
        return false;
    }
    size_t i = 0;
    PartType sum = 0;  // partial sum and carry
    while (i < a->len || i < b->len || sum) {
        PartType p = i < a->len ? a->part[i] : 0;
        PartType q = i < b->len ? b->part[i] : 0;
        sum += p + q;
        c->part[i++] = sum % UNIT;
        sum /= UNIT;
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
    printf("%u", a->part[--i]);  // no leading zeros on left-most part
    while (i--) {
        // Print in chunks of WIDTH digits
        printf(" %0"WIDTH"u", a->part[i]);
    }
    printf(" (%zu)\n", a->len);  // parts count for reference
}

int main(int argc, char *argv[])
{
    BigInt a = {0}, b = {0}, c = {0};
    setval(&a, 0);
    setval(&b, 1);
    setval(&c, 1);

    // Get requested Fibonacci number from command line
    int n = 0;
    if (argc > 1) {
        n = atoi(argv[1]);
        if (n < 0) {
            n = 0;
        }
    }
    int m = n % 3;
    n /= 3;  // while loop below is unrolled by 3

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
