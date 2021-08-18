#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CHUNK (128U)

typedef struct {
    unsigned char *digit;
    size_t len, size;
} BigInt, *pBigInt;

static void clean(pBigInt a)
{
    if (a->digit != NULL) {
        free(a->digit);
        a->digit = NULL;
    }
    a->size = 0;
    a->len = 0;
}

static bool init(pBigInt a)
{
    void *p = a->digit ? realloc(a->digit, CHUNK) : malloc(CHUNK);
    if (p == NULL) {
        clean(a);
        return false;
    }
    a->digit = (unsigned char *)p;
    a->size = CHUNK;
    a->len = 0;
    memset(a->digit, 0, CHUNK);
    return true;
}

static bool setval(pBigInt a, uint64_t val)
{
    if (!init(a)) {
        return false;
    }
    while (val) {
        a->digit[a->len++] = val % 10;
        val /= 10;
    }
    return true;
}

static bool resize(pBigInt a, size_t size)
{
    if (size == 0) {
        clean(a);
        return true;
    }
    unsigned int m = size % CHUNK;
    if (m) {
        size += CHUNK - m;
    }
    if (size < a->len) {
        return false;
    }
    if (size != a->size) {
        void *p = a->digit ? realloc(a->digit, size) : malloc(size);
        if (p == NULL) {
            return false;
        }
        a->digit = (unsigned char *)p;
        if (size > a->size) {
            memset(a->digit + a->size, 0, size - a->size);
        }
        a->size = size;
    }
    return true;
}

// a + b -> c
static bool add(pBigInt a, pBigInt b, pBigInt c)
{
    size_t maxlen = (a->len > b->len ? a->len : b->len) + 1;
    if (maxlen > c->size && !resize(c, maxlen)) {
        return false;
    }
    size_t i = 0;
    unsigned char sum = 0;
    while (i < a->len || i < b->len || sum) {
        unsigned char p = i < a->len ? a->digit[i] : 0;
        unsigned char q = i < b->len ? b->digit[i] : 0;
        sum += p + q;
        c->digit[i++] = sum % 10;
        sum /= 10;
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
    while (i--) {
        printf("%d", a->digit[i]);
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
        case 0: print(&a); break;
        case 1: print(&b); break;
        case 2: print(&c); break;
    }

    clean(&a);
    clean(&b);
    clean(&c);
    return 0;
}
