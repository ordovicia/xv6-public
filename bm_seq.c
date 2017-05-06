/*
 * Fixed size sequential malloc/free
 *
 * output is:
 *  bytes
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
#define ALLOC_NUM (1 << 22)

void* ptrs[ALLOC_NUM];

int main(void)
{
    int t, i;
    size_t s;
    int start_ticks, end_ticks, ticks;
    int malloc_ave_ticks, malloc_worst_ticks;
    int free_ave_ticks, free_worst_ticks;

    /*
    if (sbrk(1024 * 1024) < 0) {
        printf(stderr, "sbrk failed\n");
        exit();
    }
    */

    printf(1,
        "# fixed-sized sequential malloc (TEST_NUM = %d)\n", TEST_NUM);

#define max(x, y) (x > y ? x : y)

#define TICKS_END_UPDATE(AVE, WORST) \
    end_ticks = uptime();            \
    ticks = end_ticks - start_ticks; \
    AVE += ticks;                    \
    WORST = max(WORST, ticks)

#define RUN(ALLOC, FREE)                                        \
    malloc_ave_ticks = malloc_worst_ticks = 0;                  \
    free_ave_ticks = free_worst_ticks = 0;                      \
                                                                \
    for (t = 0; t < TEST_NUM; t++) {                            \
        start_ticks = uptime();                                 \
        for (i = 0; i < ALLOC_NUM; i++)                         \
            ptrs[i] = ALLOC(s);                                 \
        TICKS_END_UPDATE(malloc_ave_ticks, malloc_worst_ticks); \
                                                                \
        start_ticks = uptime();                                 \
        for (i = 0; i < ALLOC_NUM; i++)                         \
            FREE(ptrs[i]);                                      \
        TICKS_END_UPDATE(free_ave_ticks, free_worst_ticks);     \
    }                                                           \
                                                                \
    printf(1, "%d %d %d %d ",                                   \
        malloc_ave_ticks, malloc_worst_ticks,                   \
        free_ave_ticks, free_worst_ticks)

    for (s = 16; s <= 256; s += 8) {
        printf(1, "%d ", s);
        RUN(malloc, free);
        RUN(dlmalloc, dlfree);
        printf(1, "\n");
    }

    exit();
}
