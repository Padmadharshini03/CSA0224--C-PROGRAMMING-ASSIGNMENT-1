/* Task 9 alternative: slab limits and rates stored in two ARRAYS and the bill computed by a LOOP.
 * 1) proves the loop version equals the if-version for every possible consumption 0..65535
 * 2) measures the time per bill as the number of slabs s grows
 * Compile: gcc -O1 -Wall -Wextra billing_arrays.c -o billing_arrays                        */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ---- arrays: upper limit of each slab (cumulative units) and the rate in paise/unit ---- */
#define S3 3
static const unsigned LIMIT3[S3] = { 100, 200, 0xFFFFFFFFu };      /* last slab has no upper limit */
static const long     RATE3[S3]  = { 0, 225, 450 };

/* O(s): loop over the slabs; a revision only edits the two arrays */
static long energy_loop(unsigned units, const unsigned *limit, const long *rate, int s)
{
    long paise = 0;
    unsigned lower = 0;
    for (int k = 0; k < s && units > lower; k++) {
        unsigned upper = limit[k] < units ? limit[k] : units;
        paise += (long)(upper - lower) * rate[k];
        lower = limit[k];
    }
    return paise;
}

/* O(1): the fixed if-statements of billing.c (three slabs hard-wired) */
static long energy_if(unsigned units)
{
    long paise = 0;
    if (units > 200) paise += (long)(units - 200) * 450;
    if (units > 100) paise += (long)((units > 200 ? 200 : units) - 100) * 225;
    paise += (long)(units > 100 ? 100 : units) * 0;
    return paise;
}

static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec*1e-9; }

int main(void)
{
    /* 1. equivalence over the whole range of possible monthly consumption */
    long mismatches = 0;
    for (unsigned u = 0; u <= 65535; u++)
        if (energy_loop(u, LIMIT3, RATE3, S3) != energy_if(u)) mismatches++;
    printf("Equivalence test, units 0..65535 (65536 values): %ld mismatches\n\n", mismatches);

    /* 2. time per bill versus number of slabs */
    const long N = 20000000L;
    printf("%-26s %8s %14s %14s\n", "design", "slabs s", "ns per bill", "extra space");
    volatile long sink = 0;
    unsigned long long seed = 7;

    double t0 = now();
    for (long i = 0; i < N; i++) { seed = seed * 6364136223846793005ULL + 1; sink += energy_if((unsigned)((seed >> 33) % 400)); }
    double t1 = now();
    printf("%-26s %8d %14.2f %14s\n", "if-statements (fixed)", 3, (t1 - t0) / N * 1e9, "0 bytes");

    const int sizes[] = { 3, 10, 50, 200, 1000 };
    for (size_t z = 0; z < sizeof sizes / sizeof sizes[0]; z++) {
        int s = sizes[z];
        unsigned *lim = malloc((size_t)s * sizeof *lim);
        long *rate = malloc((size_t)s * sizeof *rate);
        for (int k = 0; k < s; k++) { lim[k] = (unsigned)(k + 1) * 400u / (unsigned)s + (k + 1 == s ? 0xFFFF0000u : 0); rate[k] = 100 + 10 * k; }
        t0 = now();
        for (long i = 0; i < N; i++) { seed = seed * 6364136223846793005ULL + 1; sink += energy_loop((unsigned)((seed >> 33) % 400), lim, rate, s); }
        t1 = now();
        char sp[40]; snprintf(sp, sizeof sp, "%zu bytes", (size_t)s * (sizeof *lim + sizeof *rate));
        printf("%-26s %8d %14.2f %14s\n", "arrays + loop", s, (t1 - t0) / N * 1e9, sp);
        free(lim); free(rate);
    }
    return 0;
}
