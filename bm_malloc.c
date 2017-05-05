#include "types.h"
#include "user.h"

#define stdout 1
#define stderr 2

#define TEST_NUM (1 << 3)
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
    int start_ticks, end_ticks, sum_ticks;

    randstate = uptime();
    for (i = 0; i < ALLOC_NUM; i++)
        sizes[i] = (rand() % 31 + 2 /* 2 - 32 */) * 8; /* 16 - 256 */

    printf(stdout,
        "# random small-size (krmalloc ticks, dlmalloc ticks)\n");

    for (sum_ticks = 0, j = 0; j < TEST_NUM; j++) {  // malloc
        start_ticks = uptime();
        for (i = 0; i < ALLOC_NUM; i++)
            ptrs[i] = malloc(sizes[i]);
        for (i = 0; i < ALLOC_NUM; i++)
            free(ptrs[i]);
        end_ticks = uptime();
        sum_ticks += end_ticks - start_ticks;
    }
    printf(stdout, "%d ", sum_ticks);

    for (sum_ticks = 0, j = 0; j < TEST_NUM; j++) {  // dlmalloc
        start_ticks = uptime();
        for (i = 0; i < ALLOC_NUM; i++)
            ptrs[i] = dlmalloc(sizes[i]);
        for (i = 0; i < ALLOC_NUM; i++)
            dlfree(ptrs[i]);
        end_ticks = uptime();
        sum_ticks += end_ticks - start_ticks;
    }
    printf(stdout, "%d\n", sum_ticks);

    exit();
}
