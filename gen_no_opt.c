#include "gen.c"
#include "transformations.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

char* gen_code(int size, int seed, int probs[5])
{
    int dummy;

    char* orig_code = malloc(size);
    init(orig_code, size, probs, seed, &dummy, &dummy);

    char* generated = mmap(0, size * 8, PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_SHARED | MAP_ANONYMOUS, 0, 0);

    if (generated < 0)
        perror("mmap");

    int k = 0;
    if (1) {
        char* trimmed_code = malloc(size);
        for (int i = 0; i < size; i++) {
            char instr = orig_code[i];
            if (instr == 5) //ignore BACK7
                continue;

            if ((instr == 1 || instr == 4) && orig_code[i + 1] == instr)
                continue;

            trimmed_code[k++] = instr;
        }

        size = k;
    }

    k = prolog.len;
    memcpy(generated, prolog.code, prolog.len);

    struct trans my_trans;
    for (int i = 0; i < size; i++) {
        int instr = orig_code[i];
        my_trans = transformations[instr];

        memcpy(&generated[k], my_trans.code, my_trans.len);
        k += my_trans.len;
    }

    memcpy(&generated[k], epilog.code, epilog.len);

    return generated;
}
