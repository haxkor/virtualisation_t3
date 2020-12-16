#include "gen_opt.c"
#include "iterations.c"
#include <malloc.h>
#include <time.h>

struct scenario {
    int size;
    int seed;
    int probs[5];
};

void foo(int a, int b) {}

int do_scenario(int a, int l, char* code)
{
    typedef int (*fptr_t)(int a, int l);

    return ((fptr_t)code)(a, l);
}

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

    clock_t start, duration;

    for (int i = 0; i < 8; i++) {
        if (selected && (i != selected - 1))
            continue;
        start = clock();

        char* code = gen_code(scen[i].size, scen[i].seed, scen[i].probs);
        myrand(scen[i].seed);
        int a = myrand(0) & 7;
        int l = myrand(0) & 7;
        int result = 0;

        for (int j = 0; j < ITERATIONS; j++) {
            result = do_scenario(a, l, code);
        }
        duration = clock() - start;
        printf("scenario %d took %f secs\t result=%x \n",
            i + 1, ((double)duration) / CLOCKS_PER_SEC, result);
    }
}
