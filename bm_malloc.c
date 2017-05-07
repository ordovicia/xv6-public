/*
 * Fixed size malloc on fragmented heap
 *
 * output is:
 *  log2(bytes)
 *  K&R malloc ave ticks
 *  K&R malloc worst ticks
 *  K&R free ave ticks
 *  K&R free worst ticks
 *  Doug Lea's malloc ave ticks
 *  Doug Lea's malloc worst ticks
 *  Doug Lea's free ave ticks
 *  Doug Lea's free worst ticks
 */

#include "types.h"
#include "user.h"

#define TEST_NUM (1 << 5)
#define ALLOC_NUM_LOG2 22

#define ALLOC_SIZE 16

void* ptrs[1 << ALLOC_NUM_LOG2];

int main(void)
{
    int a, t, i;
    int start_ticks, end_ticks, ticks;
    int malloc_ave_ticks, malloc_worst_ticks;
    int free_ave_ticks, free_worst_ticks;

    printf(1,
        "# fixed-size malloc on fragmented heap (TEST_NUM = %d)\n", TEST_NUM);

#define max(x, y) (x > y ? x : y)

#define TICKS_END_UPDATE(AVE, WORST) \
    end_ticks = uptime();            \
    ticks = end_ticks - start_ticks; \
    AVE += ticks;                    \
    WORST = max(WORST, ticks)

#define RUN(ALLOC_NUM, ALLOC, FREE)                             \
    malloc_ave_ticks = malloc_worst_ticks = 0;                  \
    free_ave_ticks = free_worst_ticks = 0;                      \
                                                                \
    for (t = 0; t < TEST_NUM; t++) {                            \
        ptrs[0] = ALLOC(2 * ALLOC_SIZE);                        \
        for (i = 1; i < ALLOC_NUM; i++)                         \
            ptrs[i] = ALLOC(ALLOC_SIZE);                        \
        for (i = 0; i < ALLOC_NUM; i += 2)                      \
            FREE(ptrs[i]);                                      \
                                                                \
        start_ticks = uptime();                                 \
        ptrs[0] = ALLOC(2 * ALLOC_SIZE);                        \
        TICKS_END_UPDATE(malloc_ave_ticks, malloc_worst_ticks); \
                                                                \
        FREE(ptrs[ALLOC_NUM - 1]);                              \
        start_ticks = uptime();                                 \
        FREE(ptrs[0]);                                          \
        TICKS_END_UPDATE(free_ave_ticks, free_worst_ticks);     \
                                                                \
        for (i = 1; i < ALLOC_NUM - 1; i += 2)                  \
            FREE(ptrs[i]);                                      \
    }                                                           \
                                                                \
    printf(1, "%d %d %d %d ",                                   \
        malloc_ave_ticks, malloc_worst_ticks,                   \
        free_ave_ticks, free_worst_ticks)

    for (a = 8; a <= ALLOC_NUM_LOG2; a++) {
        printf(1, "%d ", a);
        RUN((1 << a), malloc, free);
        RUN((1 << a), dlmalloc, dlfree);
        printf(1, "\n");
    }

    exit();
}
