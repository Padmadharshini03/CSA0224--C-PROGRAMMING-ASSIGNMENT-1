/* Task 10 alternative: time per addition and space per variable for int / long long / float / double accumulators.
 * Compile: gcc -O1 accumulate_bench.c -o accumulate_bench                                                   */
#include <stdio.h>
#include <time.h>
static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec*1e-9; }
int main(void)
{
    const long N = 400000000L;
    volatile int vi = 3; volatile long long vl = 3; volatile float vf = 3.0f; volatile double vd = 3.0;
    int  si = 0;  long long sl = 0;  float sf = 0;  double sd = 0;
    int a = vi; long long b = vl; float c = vf; double d = vd;
    double t0 = now();
    for (long i = 0; i < N; i++) { si = (int)((unsigned)si + (unsigned)a); __asm__ volatile("" : "+r"(si)); }
    double t1 = now();
    for (long i = 0; i < N; i++) { sl += b; __asm__ volatile("" : "+r"(sl)); }
    double t2 = now();
    for (long i = 0; i < N; i++) { sf += c; __asm__ volatile("" : "+x"(sf)); }
    double t3 = now();
    for (long i = 0; i < N; i++) { sd += d; __asm__ volatile("" : "+x"(sd)); }
    double t4 = now();
    printf("%-12s %6s %14s\n", "accumulator", "bytes", "ns/addition");
    printf("%-12s %6zu %14.3f\n", "int",       sizeof si, (t1 - t0) / N * 1e9);
    printf("%-12s %6zu %14.3f\n", "long long", sizeof sl, (t2 - t1) / N * 1e9);
    printf("%-12s %6zu %14.3f\n", "float",     sizeof sf, (t3 - t2) / N * 1e9);
    printf("%-12s %6zu %14.3f\n", "double",    sizeof sd, (t4 - t3) / N * 1e9);
    printf("(checksums %d %lld %.0f %.0f)\n", si, sl, (double)sf, sd);
    return 0;
}
