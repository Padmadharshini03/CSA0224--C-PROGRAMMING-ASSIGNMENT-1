/* Task 10 - compare expectation with execution.
 * Generates n bills (4,000 village / 60,000 taluk / 800,000 district) from a fixed pseudo-random
 * list of units (0..899, so that every slab is exercised) and accumulates the total in
 *   - float  rupees          - double rupees          - int, long, long long paise (overflow-checked)
 * Compile: gcc -O1 -Wall -Wextra compare.c -o compare -lm                                         */
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>

static long bill_paise(int units)
{
    long p = 5000L;
    if (units > 200) p += (long)(units - 200) * 450L;
    if (units > 100) p += (long)((units > 200 ? 200 : units) - 100) * 225L;
    return p;
}

int main(void)
{
    const long ns[] = { 4000, 60000, 800000 };
    long long district_total = 0;                          /* exact total of the 800,000 bills (paise) */

    printf("=== A. float vs paise totals ===\n");
    printf("%8s | %18s | %18s %14s | %18s %12s\n", "n bills", "exact paise (Rs)", "float Rs total", "error (Rs)", "double Rs total", "error (Rs)");
    for (int k = 0; k < 3; k++) {
        long n = ns[k];
        long long ex = 0;
        float sf = 0.0f;  double sd = 0.0;
        unsigned long long seed = 2026;
        for (long i = 0; i < n; i++) {
            seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
            int units = (int)((seed >> 33) % 900);
            long p = bill_paise(units);
            ex += p;
            sf += (float)p / 100.0f;                  /* float rupees: 162.50 etc */
            sd += (double)p / 100.0;
        }
        double ex_rs = (double)ex / 100.0;
        if (k == 2) district_total = ex;
        printf("%8ld | %18.2f | %18.2f %14.2f | %18.4f %12.8f\n", n, ex_rs, (double)sf, (double)sf - ex_rs, sd, sd - ex_rs);
    }

    printf("\n=== B. does the total overflow?  (paise, overflow detected with __builtin_add_overflow) ===\n");
    printf("sizeof(int)=%zu  sizeof(long)=%zu  sizeof(long long)=%zu  -> int max %d, long max %ld\n",
           sizeof(int), sizeof(long), sizeof(long long), INT_MAX, LONG_MAX);
    printf("%8s | %20s | %-22s %-22s %-22s\n", "n bills", "true total (paise)", "int total", "long total", "long long total");
    for (int k = 0; k < 3; k++) {
        long n = ns[k];
        int       ti = 0;  long tl = 0;  long long tll = 0;
        int ovf_i = 0, ovf_l = 0, ovf_ll = 0;
        unsigned long long seed = 2026;
        long first_ovf_i = -1;
        for (long i = 0; i < n; i++) {
            seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
            int units = (int)((seed >> 33) % 900);
            long p = bill_paise(units);
            if (__builtin_add_overflow(ti, (int)p, &ti))       { if (!ovf_i) first_ovf_i = i + 1; ovf_i = 1; }
            if (__builtin_add_overflow(tl, p, &tl))            ovf_l = 1;
            if (__builtin_add_overflow(tll, (long long)p, &tll)) ovf_ll = 1;
        }
        char a[40], b[40], c[40];
        snprintf(a, sizeof a, "%d%s", ti, ovf_i ? " OVERFLOW" : " ok");
        snprintf(b, sizeof b, "%ld%s", tl, ovf_l ? " OVERFLOW" : " ok");
        snprintf(c, sizeof c, "%lld%s", tll, ovf_ll ? " OVERFLOW" : " ok");
        printf("%8ld | %20lld | %-22s %-22s %-22s\n", n, tll, a, b, c);
        if (ovf_i) printf("          (the int total first overflowed at bill number %ld)\n", first_ovf_i);
    }
    printf("\nint holds at most %d paise = Rs %.2f  (about Rs %.1f crore)\n", INT_MAX, INT_MAX / 100.0, INT_MAX / 1e9);
    printf("A 32-bit long (Windows) behaves like int; here long is %zu bits.\n", sizeof(long) * 8);

    printf("\n=== C. sizes on this system vs minimum guaranteed by the C standard ===\n");
    printf("%-12s %12s %14s %16s %18s\n", "type", "sizeof here", "bits here", "standard min bits", "standard min range");
    printf("%-12s %12zu %14zu %16d %18s\n", "char",      sizeof(char),      sizeof(char) * CHAR_BIT,      8,  "-127..127 / 0..255");
    printf("%-12s %12zu %14zu %16d %18s\n", "short",     sizeof(short),     sizeof(short) * CHAR_BIT,     16, "-32767..32767");
    printf("%-12s %12zu %14zu %16d %18s\n", "int",       sizeof(int),       sizeof(int) * CHAR_BIT,       16, "-32767..32767");
    printf("%-12s %12zu %14zu %16d %18s\n", "long",      sizeof(long),      sizeof(long) * CHAR_BIT,      32, "+-2147483647");
    printf("%-12s %12zu %14zu %16d %18s\n", "long long", sizeof(long long), sizeof(long long) * CHAR_BIT, 64, "+-9.22e18");
    printf("%-12s %12zu %14zu %16s %18s\n", "float",     sizeof(float),     sizeof(float) * CHAR_BIT,     "-", "6 digits (FLT_DIG)");
    printf("%-12s %12zu %14zu %16s %18s\n", "double",    sizeof(double),    sizeof(double) * CHAR_BIT,    "-", "10 digits (DBL_DIG)");
    printf("FLT_DIG=%d (std min 6)   DBL_DIG=%d (std min 10)   CHAR_BIT=%d\n", FLT_DIG, DBL_DIG, CHAR_BIT);

    printf("\n=== D. when does a double accumulator lose accuracy? ===\n");
    double two53 = 9007199254740992.0;                     /* 2^53 */
    printf("2^53 = %.0f paise = Rs %.0f lakh crore (integers above this are not all representable)\n", two53, two53 / 100.0 / 1e12);
    printf("(double)9007199254740993 prints as %.0f   (the +1 is lost)\n", (double)9007199254740993LL);
    printf("The district total above = %lld paise = %.4f%% of 2^53, so a double accumulating PAISE is still exact\n",
           district_total, (double)district_total / two53 * 100.0);
    printf("Number of Rs 162.35 bills needed to exceed 2^53 paise: %.2e\n", two53 / 16235.0);
    printf("float: integers above 2^24 = 16777216 are not all representable; the district total in rupees (%.3g) is %.1f x larger\n", (double)district_total / 100.0, (double)district_total / 100.0 / 16777216.0);
    return 0;
}
