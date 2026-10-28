/* Task 7 / Task 10 - alternative design: do ALL arithmetic in floating point and round only
 * once at the end, versus exact integer paise.  Measures how the error in the total grows with n.
 * Two tariffs: the original one (all rates exactly representable in binary: 2.25, 4.50, 50.00)
 *              the revised one  (Rs 2.30 and Rs 4.60: NOT exactly representable).
 * Compile: gcc -O1 -Wall -Wextra error_growth.c -o error_growth -lm                              */
#include <stdio.h>
#include <math.h>
#include <time.h>

static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec*1e-9; }

/* exact integer bill in paise for a given rate2 / rate3 (paise per unit) */
static long bill_paise(int units, long r2, long r3)
{
    long p = 5000L;
    if (units > 200) p += (long)(units - 200) * r3;
    if (units > 100) p += (long)((units > 200 ? 200 : units) - 100) * r2;
    return p;
}
static float bill_float(int units, float r2, float r3)
{
    float p = 50.0f;
    if (units > 200) p += (float)(units - 200) * r3;
    if (units > 100) p += (float)((units > 200 ? 200 : units) - 100) * r2;
    return p;
}
static double bill_double(int units, double r2, double r3)
{
    double p = 50.0;
    if (units > 200) p += (double)(units - 200) * r3;
    if (units > 100) p += (double)((units > 200 ? 200 : units) - 100) * r2;
    return p;
}

int main(void)
{
    const long ns[] = { 1000, 4000, 10000, 60000, 100000, 800000, 3000000, 10000000 };
    const struct { const char *name; long r2, r3; } tariffs[] = {
        { "ORIGINAL tariff  (2.25 / 4.50)", 225, 450 },
        { "REVISED  tariff  (2.30 / 4.60)", 230, 460 }
    };

    for (int t = 0; t < 2; t++) {
        printf("\n%s\n", tariffs[t].name);
        printf("%9s | %16s | %14s %13s | %14s %13s\n",
               "n bills", "exact (Rs)", "float total", "error (Rs)", "double total", "error (Rs)");
        float  fr2 = (float)tariffs[t].r2 / 100.0f,  fr3 = (float)tariffs[t].r3 / 100.0f;
        double dr2 = (double)tariffs[t].r2 / 100.0,  dr3 = (double)tariffs[t].r3 / 100.0;
        for (size_t k = 0; k < sizeof ns / sizeof ns[0]; k++) {
            long n = ns[k];
            long long exact = 0;
            float  sf = 0.0f;
            double sd = 0.0;
            unsigned long long seed = 12345;
            for (long i = 0; i < n; i++) {
                seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
                int units = (int)((seed >> 33) % 400);                    /* 0..399 units */
                exact += bill_paise(units, tariffs[t].r2, tariffs[t].r3);
                sf    += bill_float(units, fr2, fr3);
                sd    += bill_double(units, dr2, dr3);
            }
            double exact_rs = (double)exact / 100.0;
            double ef = (double)sf - exact_rs;
            double ed = sd - exact_rs;
            printf("%9ld | %16.2f | %14.2f %13.4f | %14.4f %13.8f\n", n, exact_rs, (double)sf, ef, sd, ed);
        }
    }

    /* time and space per value */
    printf("\nTime per bill (ns) for the three representations, 20 million bills:\n");
    const long N = 20000000L;
    long long ex = 0; float sf = 0; double sd = 0;
    unsigned long long seed = 99;
    int *u = NULL; (void)u;
    double t0 = now();
    for (long i = 0; i < N; i++) { seed = seed * 6364136223846793005ULL + 1; ex += bill_paise((int)((seed >> 33) % 400), 225, 450); }
    double t1 = now();
    for (long i = 0; i < N; i++) { seed = seed * 6364136223846793005ULL + 1; sf += bill_float((int)((seed >> 33) % 400), 2.25f, 4.5f); }
    double t2 = now();
    for (long i = 0; i < N; i++) { seed = seed * 6364136223846793005ULL + 1; sd += bill_double((int)((seed >> 33) % 400), 2.25, 4.5); }
    double t3 = now();
    printf("  long paise : %6.2f ns/bill   (%zu bytes per value)\n", (t1 - t0) / N * 1e9, sizeof(long));
    printf("  float Rs   : %6.2f ns/bill   (%zu bytes per value)\n", (t2 - t1) / N * 1e9, sizeof(float));
    printf("  double Rs  : %6.2f ns/bill   (%zu bytes per value)\n", (t3 - t2) / N * 1e9, sizeof(double));
    printf("  (checksums %lld %.1f %.1f)\n", ex, (double)sf, sd);
    return 0;
}
