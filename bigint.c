#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define UNIT  (1000000U)
#define WIDTH "6"
#define CHUNK (1024U)

typedef struct {
    uint32_t *part;
    size_t    len, maxlen;
} BigInt, *pBigInt;

static void clean(pBigInt a)
{
    if (a->part != NULL) {
        free(a->part);
        a->part = NULL;
    }
    a->maxlen = 0;
    a->len = 0;
}

static bool init(pBigInt a)
{
    size_t memsize = CHUNK * sizeof *a->part;
    void *p = a->part ? realloc(a->part, memsize) : malloc(memsize);
    if (p == NULL) {
        clean(a);
        return false;
    }
    a->part = (uint32_t *)p;
    a->maxlen = CHUNK;
    a->len = 0;
    memset(a->part, 0, memsize);
    return true;
}

static bool setval(pBigInt a, uint64_t val)
{
    if (!init(a)) {
        return false;
    }
    while (val) {
        a->part[a->len++] = val % UNIT;
        val /= UNIT;
    }
    return true;
}

static bool resize(pBigInt a, size_t newlen)
{
    if (newlen == 0) {
        clean(a);
        return true;
    }
    unsigned int m = newlen % CHUNK;
    if (m) {
        newlen += CHUNK - m;
    }
    if (newlen < a->len) {
        return false;
    }
    if (newlen != a->maxlen) {
        size_t memsize = newlen * sizeof *a->part;
        void *p = a->part ? realloc(a->part, memsize) : malloc(memsize);
        if (p == NULL) {
            return false;
        }
        a->part = (uint32_t *)p;
        if (newlen > a->maxlen) {
            memset(a->part + a->maxlen, 0, (newlen - a->maxlen) * sizeof *a->part);
        }
        a->maxlen = newlen;
    }
    return true;
}

// a + b -> c
static bool add(pBigInt a, pBigInt b, pBigInt c)
{
    size_t maxlen = (a->len > b->len ? a->len : b->len) + 1;
    if (maxlen > c->maxlen && !resize(c, maxlen)) {
        return false;
    }
    size_t i = 0;
    uint32_t sum = 0;
    while (i < a->len || i < b->len || sum) {
        uint32_t p = i < a->len ? a->part[i] : 0;
        uint32_t q = i < b->len ? b->part[i] : 0;
        sum += p + q;
        c->part[i++] = sum % UNIT;
        sum /= UNIT;
    }
    c->len = i;
    return true;
}

static void print(pBigInt a)
{
    size_t i = a->len;
    if (i == 0) {
        printf("0\n");
        return;
    }
    printf("%u", a->part[--i]);  // no leading zeros on left-most part
    while (i--) {
        printf(" %0"WIDTH"u", a->part[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    BigInt a = {0}, b = {0}, c = {0};
    setval(&a, 0);
    setval(&b, 1);
    init(&c);

    int n = 0;
    if (argc > 1) {
        n = atoi(argv[1]);
        if (n < 0) {
            n = 0;
        }
    }

    unsigned int i = 0;
    while (n--) {
        switch (i) {
            case 0: add(&a, &b, &c); i = 1; break;
            case 1: add(&b, &c, &a); i = 2; break;
            case 2: add(&c, &a, &b); i = 0; break;
        }
    }
    switch (i) {
        case 0: print(&a); printf("%zu", a.len); break;
        case 1: print(&b); printf("%zu", b.len); break;
        case 2: print(&c); printf("%zu", c.len); break;
    }

    clean(&a);
    clean(&b);
    clean(&c);
    return 0;
}
