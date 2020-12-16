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

enum opcode { OP_INC9A = 6,
    OP_INC6A,
    OP_CLRA_SKIP1,
    OP_SETA3 };

int predecode(char* code, int size)
{
    for (int i = 0; i < size - 3; i++) {
        if (code[i] == 2 && 2 == code[i + 1]) {
            if (code[i + 2] == 2) {
                code[i] = OP_INC9A;
                i += 2;
            } else {
                code[i] = OP_INC6A;
                i += 2;
            }
        }

        if (code[i] == 2 && (code[i + 1] == 1 || code[i + 1] == 3)) {
            code[i] = OP_CLRA_SKIP1;
            i += 1;
        }

        if (code[i] == 1 && code[i + 1] == 2) {
            code[i] = OP_SETA3;
            i += 1;
        }
    }
    return 0;
}

int do_scenario(int size, int seed, int probs[5], char* code)
{
    static void* dispatch_table[] = {
        &&HALT, &&CLEAR_A, &&INC3A, &&DECA,
        &&SETL, &&BACK7,
        &&INC9A, &&INC6A, &&CLRA_SKIP1, &&SETA3
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

INC9A:
    a += 9;
    ip += 2;
    GO_NEXT();

INC6A:
    a += 6;
    ip += 1;
    GO_NEXT();

CLRA_SKIP1:
    a = 0;
    ip += 1;
    GO_NEXT();

SETA3:
    a = 3;
    ip += 1;
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
        predecode(code, scen[i].size);

        start = clock();

        for (int j = 0; j < ITERATIONS; j++) {
            do_scenario(scen[i].size, scen[i].seed, scen[i].probs, code);
        }
        duration = clock() - start;
        printf("scenario %d took %f secs\n", i + 1, ((double)duration) / CLOCKS_PER_SEC);
    }
}
