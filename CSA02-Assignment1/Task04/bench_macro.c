/* Task 4: time per use of an expanded macro versus a function call.
 * Compile twice:  gcc -O0 bench_macro.c -o bench_O0     gcc -O2 bench_macro.c -o bench_O2   */
#include <stdio.h>
#include <time.h>

#define MAX_M(a, b) (((a) > (b)) ? (a) : (b))
static inline int max_f(int a, int b) { return (a > b) ? a : b; }
__attribute__((noinline)) static int max_call(int a, int b) { return (a > b) ? a : b; }

static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec*1e-9; }

int main(void)
{
    const long N = 400000000L;
    volatile int vb = 7;
    long s1 = 0, s2 = 0, s3 = 0;
    double t0 = now();
    for (long i = 0; i < N; i++) s1 += MAX_M((int)(i & 15), vb);
    double t1 = now();
    for (long i = 0; i < N; i++) s2 += max_f((int)(i & 15), vb);
    double t2 = now();
    for (long i = 0; i < N; i++) s3 += max_call((int)(i & 15), vb);
    double t3 = now();
    printf("N = %ld iterations\n", N);
    printf("macro MAX            : %6.3f ns/use\n", (t1 - t0) / N * 1e9);
    printf("static inline MAX    : %6.3f ns/use\n", (t2 - t1) / N * 1e9);
    printf("noinline function    : %6.3f ns/use\n", (t3 - t2) / N * 1e9);
    printf("(checksums %ld %ld %ld)\n", s1, s2, s3);
    return 0;
}
