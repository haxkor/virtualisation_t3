#include "gen.c"
#include "transformations.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define SKIP_REDUNDANT_INSTR 1
char* gen_code(int size, int seed, int probs[5])
{
    int dummy;

    char* orig_code = malloc(size);
    init(orig_code, size, probs, seed, &dummy, &dummy);

    int k = 0;
    if (SKIP_REDUNDANT_INSTR) {
        char* trimmed_code = malloc(size);
        for (int i = 0; i < size; i++) {
            char instr = orig_code[i];
            if (instr == 5) //ignore BACK7
                continue;

            if (0 && (instr == 1 || instr == 4) && orig_code[i + 1] == instr)
                continue;

            trimmed_code[k++] = instr;
        }
        size = k;
        free(orig_code);
        orig_code = trimmed_code;
    }

    char* generated = mmap(0, size * 8, PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_SHARED | MAP_ANONYMOUS, 0, 0);

    if (generated < 0)
        perror("mmap");

    k = prolog.len;
    memcpy(generated, prolog.code, prolog.len);

    struct trans my_trans;
    for (int i = 0; i < size; i++) {
        int trans_index;
        char orig_instr = orig_code[i];

        if (orig_instr == 0 || orig_instr == 5)
            continue;

        switch (orig_instr) {
        case 1: // clear A
            switch (orig_code[i + 1]) {
            case 1: //skip another clear a
                i++;
                trans_index = 1;
                break;
            case 2: //combine to "set A=3"
                i++;
                trans_index = SETA3_INDEX;
                break;
            case 3: //combine to "set a=-1"
                i++;
                trans_index = SETAM1_INDEX;
                break;

            case 4: //clear both registers
                i++;
                trans_index = CLRBOTH_INDEX;
                break;

            default:
                trans_index = orig_instr;
                break;
            }
            break;

        case 2: //ADD3A, most common instruction
            switch (orig_code[i + 1]) {
            case 1: // clear A
                i++;
                trans_index = 1;
                break;

            case 2: //another ADD3, most common
                if (orig_code[i + 2] == 2) {
                    i += 2;
                    trans_index = ADD9_INDEX;
                } else {
                    i++;
                    trans_index = ADD6_INDEX;
                }
                break;
            case 3: //DECA
                i++;
                trans_index = ADD2_INDEX;
                break;
            default:
                trans_index = orig_instr;
                break;
            }
            break;

        default:
            trans_index = orig_instr;
            break;
        }
        assert(trans_index != -1);
        //printf("trans_index = %d\n", trans_index);

        my_trans = transformations[trans_index];

        memcpy(&generated[k], my_trans.code, my_trans.len);
        k += my_trans.len;
    }

    memcpy(&generated[k], epilog.code, epilog.len);

    return generated;
}
