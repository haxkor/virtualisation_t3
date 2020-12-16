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
    int a, l, ip = 0;

    // init(code, size, probs, seed, &a, &l);

    myrand(seed);
    a = myrand(0) & 7;
    l = myrand(0) & 7;

    while (1) {
        switch (code[ip++]) {

        case 0:
            return a;

        case 1:
            a = 0;
            break;

        case 2:
            a += 3;
            break;

        case 3:
            a -= 1;
            break;

        case 4:
            l = a;
            break;

        case 5:
            l -= 1;
            ip = (l > 0) ? ip - 7 : ip;
            break;
        }
    }
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

    for (int i = 0; i < 8; i++) {
        if (selected && (i != selected - 1))
            continue;

        clock_t start, duration;

        char* code = gen_code(scen[i].size, scen[i].seed, scen[i].probs);

        start = clock();
        int result;

        for (int j = 0; j < ITERATIONS; j++) {
            result = do_scenario(scen[i].size, scen[i].seed, scen[i].probs, code);
        }
        duration = clock() - start;
        printf("scenario %d took %f secs, result=%x \n", i + 1, ((double)duration) / CLOCKS_PER_SEC, result);
    }
}
