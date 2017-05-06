#include "types.h"
#include "user.h"

#define stdout 1
#define stderr 2

#define TEST_NUM (1 << 10)
#define ALLOC_NUM (1 << 18)

size_t sizes[ALLOC_NUM];
void* ptrs[ALLOC_NUM];

size_t randstate;
size_t rand()
{
    randstate = randstate * 1664525 + 1013904223;
    return randstate;
}

int main(void)
{
    int i, j;
    size_t s;
    int start_ticks, end_ticks, sum_ticks, ave_ticks, worst_ticks;

    printf(stdout, "# small size malloc\n");
    for (s = 16; s <= 512; s += 8) {
        printf(stdout, "%d ", s);

        ave_ticks = 0;
        worst_ticks = 0;
        for (j = 0; j < TEST_NUM; j++) {  // malloc
            start_ticks = uptime();
            for (i = 0; i < ALLOC_NUM; i++)
                ptrs[i] = malloc(s);
            end_ticks = uptime();
            sum_ticks = end_ticks - start_ticks;
            ave_ticks += sum_ticks;
            worst_ticks = worst_ticks < sum_ticks ? sum_ticks : worst_ticks;

            for (i = 0; i < ALLOC_NUM; i++)
                free(ptrs[i]);
        }

        printf(stdout, "%d %d ", ave_ticks / TEST_NUM, worst_ticks);

        ave_ticks = 0;
        worst_ticks = 0;
        for (j = 0; j < TEST_NUM; j++) {  // dlmalloc
            start_ticks = uptime();
            for (i = 0; i < ALLOC_NUM; i++)
                ptrs[i] = dlmalloc(s);
            end_ticks = uptime();
            sum_ticks = end_ticks - start_ticks;
            ave_ticks += sum_ticks;
            worst_ticks = worst_ticks < sum_ticks ? sum_ticks : worst_ticks;

            for (i = 0; i < ALLOC_NUM; i++)
                dlfree(ptrs[i]);
        }

        printf(stdout, "%d %d\n", ave_ticks / TEST_NUM, worst_ticks);
    }

#if 0
    printf(stdout, "# random small size\n");
    randstate = uptime();
    for (i = 0; i < ALLOC_NUM; i++)
        sizes[i] = (rand() % 31 + 2 /* 2 - 32 */) * 8; /* 16 - 256 */

    for (sum_ticks = 0, j = 0; j < TEST_NUM; j++) {  // malloc
        start_ticks = uptime();
        for (i = 0; i < ALLOC_NUM; i++)
            ptrs[i] = malloc(sizes[i]);
        for (i = ALLOC_NUM - 1; i >= 0; i--)
            free(ptrs[i]);
        end_ticks = uptime();
        sum_ticks += end_ticks - start_ticks;
    }
    printf(stdout, "%d ", sum_ticks);

    for (sum_ticks = 0, j = 0; j < TEST_NUM; j++) {  // dlmalloc
        start_ticks = uptime();
        for (i = 0; i < ALLOC_NUM; i++)
            ptrs[i] = dlmalloc(sizes[i]);
        for (i = ALLOC_NUM - 1; i >= 0; i--)
            dlfree(ptrs[i]);
        end_ticks = uptime();
        sum_ticks += end_ticks - start_ticks;
    }
    printf(stdout, "%d\n", sum_ticks);
#endif

    exit();
}
