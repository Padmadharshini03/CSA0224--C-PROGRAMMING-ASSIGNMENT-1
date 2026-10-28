/* Task 11: start-up time to read t tariff values from a file, and the memory they occupy.
 * Compile: gcc -O2 -DNO_MAIN bench_cfg.c -o bench_cfg    (needs MAX_SLABS raised, done below)    */
#define NO_MAIN
#include <time.h>
#define MAX_SLABS 70000
#include "billing_cfg.c"
static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec*1e-9; }
static Tariff T;
int main(void)
{
    const int sizes[] = { 3, 30, 300, 3000, 30000, 60000 };
    printf("%10s %12s %14s %16s %14s\n", "slabs t", "file bytes", "load time (us)", "us per value", "memory (bytes)");
    for (size_t z = 0; z < sizeof sizes / sizeof sizes[0]; z++) {
        int t = sizes[z];
        char path[64]; snprintf(path, sizeof path, "/tmp/cfg_%d.cfg", t);
        FILE *fp = fopen(path, "w");
        fprintf(fp, "fixed_charge_paise = 5000\n");
        for (int k = 0; k < t - 1; k++) fprintf(fp, "slab = %d, %d\n", (k + 1) * 10, 100 + k % 50);
        fprintf(fp, "slab = 0, 450\n");
        fclose(fp);
        fp = fopen(path, "r"); fseek(fp, 0, SEEK_END); long bytes = ftell(fp); fclose(fp);
        int reps = t <= 300 ? 20000 : (t <= 3000 ? 2000 : 100);
        char err[100];
        double t0 = now();
        for (int r = 0; r < reps; r++) if (load_tariff(path, &T, err, sizeof err)) { puts(err); return 1; }
        double us = (now() - t0) / reps * 1e6;
        printf("%10d %12ld %14.2f %16.3f %14zu\n", t, bytes, us, us / (2.0 * t), (size_t)t * (sizeof T.limit[0] + sizeof T.rate[0]) + sizeof(long) + sizeof(int));
    }
    return 0;
}
