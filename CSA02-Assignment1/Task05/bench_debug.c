/* Task 5: cost of debug code that is switched OFF.
 *   gcc -O1 bench_debug.c -o bench_ifdef           (DBG compiled away)
 *   gcc -O1 -DRUNTIME_FLAG bench_debug.c -o bench_flag   (if (debug) test on every call)  */
#include <stdio.h>
#include <time.h>
#include "tariff.h"

#ifdef RUNTIME_FLAG
  volatile int debug = 0;                       /* volatile: value can change at run time */
  #define LOG(...) do { if (debug) fprintf(stderr, "[DEBUG] " __VA_ARGS__); } while (0)
#else
  #define LOG(...) DBG(__VA_ARGS__)             /* expands to ((void)0) */
#endif

__attribute__((noinline)) long energy(unsigned int units)
{
    long paise = 0;
    if (units > SLAB2_UNITS) paise += (long)(units - SLAB2_UNITS) * RATE3_PAISE;
    if (units > SLAB1_UNITS) paise += (long)((units > SLAB2_UNITS ? SLAB2_UNITS : units) - SLAB1_UNITS) * RATE2_PAISE;
    LOG("energy: units=%u -> %ld paise (this is a long debug message with some text)\n", units, paise);
    return paise;
}

int main(void)
{
    const long N = 300000000L;
    long sum = 0;
    struct timespec a, b;
    clock_gettime(CLOCK_MONOTONIC, &a);
    for (long i = 0; i < N; i++) sum += energy((unsigned)(i % 400));
    clock_gettime(CLOCK_MONOTONIC, &b);
    double ns = ((b.tv_sec - a.tv_sec) + (b.tv_nsec - a.tv_nsec) * 1e-9) / N * 1e9;
    printf("%.3f ns per call (sum=%ld)\n", ns, sum);
    return 0;
}
