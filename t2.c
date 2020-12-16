#include "gen.c"
#include "iterations.c"
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

struct state {
    int a;
    int l;
    int ip;
};

struct scenario {
    int size;
    int seed;
    int probs[5];
};

char* gen_code(int size, int seed, int probs[5])
{
    char* code = malloc(size);
    int a;
    init(code, size, probs, seed, &a, &a);

    return code;
}
int do_scenario(int size, int seed, int probs[5], char* code)
{
    static void* dispatch_table[] = {
        &&HALT, &&CLEAR_A, &&INC3A, &&DECA,
        &&SETL, &&BACK7
    };

#define GO_NEXT()                         \
    {                                     \
        goto* dispatch_table[code[ip++]]; \
    }

    int a, l, ip = 0;

    // init(code, size, probs, seed, &a, &l);

    myrand(seed);
    a = myrand(0) & 7;
    l = myrand(0) & 7;

    GO_NEXT();

HALT:
    return a;

CLEAR_A:
    a = 0;
    GO_NEXT();

INC3A:
    a += 3;
    GO_NEXT();

DECA:
    a -= 1;
    GO_NEXT();

SETL:
    l = a;
    GO_NEXT();

BACK7:
    l -= 1;
    GO_NEXT();
}

#define SCENARIOS 6

struct scenario scen[SCENARIOS];

int main(int argc, char** argv)
{
    int selected = 0;
    if (argc > 1)
        selected = atoi(argv[1]);

    struct scenario scen[8] = {
        { 10000, 1, { 0, 1, 0, 0, 0 } },
        { 10000, 1, { 1, 1, 1, 0, 0 } },

        { 10000, 1, { 1, 9, 1, 5, 5 } },
        { 10000, 2, { 1, 9, 1, 5, 5 } },
        { 10000, 3, { 1, 9, 1, 5, 5 } },

        { 50000, 1, { 1, 9, 1, 5, 5 } },
        { 50000, 2, { 1, 9, 1, 5, 5 } },
        { 50000, 3, { 1, 9, 1, 5, 5 } },
    };

    double timings[SCENARIOS][ITERATIONS];
    for (int i = 0; i < 8; i++) {
        if (selected && (i != selected - 1))
            continue;

        clock_t start, duration;

        char* code = gen_code(scen[i].size, scen[i].seed, scen[i].probs);

        start = clock();

        for (int j = 0; j < ITERATIONS; j++) {
            do_scenario(scen[i].size, scen[i].seed, scen[i].probs, code);
        }
        duration = clock() - start;
        printf("scenario %d took %f secs\n", i + 1, ((double)duration) / CLOCKS_PER_SEC);
    }
}
