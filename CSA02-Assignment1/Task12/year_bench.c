/* Task 12-D : yearly totals (12 monthly bills, 35,040 readings) for one household, and throughput.
 * Compile: gcc -O2 -Wall -Wextra year_bench.c smart.c -o year_bench                                  */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include "smart.h"

static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec*1e-9; }

/* build `days` of readings (+1 opening value); same daily shape as gen_readings "profile" */
static uint32_t *make_month(unsigned days, uint32_t opening, uint64_t *seed, size_t *n)
{
    size_t slots = (size_t)days * 96;
    uint32_t *r = malloc((slots + 1) * sizeof *r);
    r[0] = opening;
    for (size_t i = 0; i < slots; i++) {
        unsigned hour = (unsigned)(i % 96) / 4;
        *seed = *seed * 6364136223846793005ULL + 1442695040888963407ULL;
        unsigned base = (hour < 5) ? 20 : (hour < 9) ? 90 : (hour < 17) ? 45 : (hour < 22) ? 160 : 60;
        r[i + 1] = r[i] + base + (unsigned)((*seed >> 33) % 21);
    }
    *n = slots + 1;
    return r;
}

int main(void)
{
    Tariff t; char err[200];
    if (tariff_load("tariff_smart.cfg", &t, err, sizeof err)) { puts(err); return 1; }

    static const unsigned mdays[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    static const char *mname[12] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
    uint64_t seed = 42, year_wh = 0, year_paise = 0, year_readings = 0, year_u = 0;
    uint32_t opening = 4294000000u;                          /* near the 32-bit limit: a roll-over happens during the year */

    printf("%-5s %9s %12s %12s %12s\n", "month", "readings", "energy kWh", "peak kWh", "bill (Rs)");
    for (int m = 0; m < 12; m++) {
        size_t n; uint32_t *r = make_month(mdays[m], opening, &seed, &n);
        MonthBill b;
        if (bill_month(&t, r, n, &b, err, sizeof err)) { puts(err); return 1; }
        printf("%-5s %9" PRIu64 " %8" PRIu64 ".%03" PRIu64 " %8" PRIu64 ".%03" PRIu64 " %12" PRIu64 ".%02" PRIu64 "\n",
               mname[m], b.intervals, b.total_wh / 1000, b.total_wh % 1000, b.peak_wh / 1000, b.peak_wh % 1000,
               b.total_paise / 100, b.total_paise % 100);
        year_wh += b.total_wh; year_paise += b.total_paise; year_readings += b.intervals; year_u += b.total_u;
        opening = r[n - 1];
        free(r);
    }
    printf("YEAR  %9" PRIu64 " %8" PRIu64 ".%03" PRIu64 " %12s %12" PRIu64 ".%02" PRIu64 "\n", year_readings,
           year_wh / 1000, year_wh % 1000, "", year_paise / 100, year_paise % 100);
    printf("(one household-year = %" PRIu64 " Wh fits uint32; 800,000 such households = %.3e Wh > 2^32 = 4.295e9, so district totals need uint64_t)\n",
           year_wh, (double)year_wh * 800000.0);

    /* ---------- throughput ---------- */
    size_t n; uint64_t s2 = 7;
    uint32_t *r = make_month(30, 1000, &s2, &n);
    MonthBill b;
    const int REPS = 4000;
    double t0 = now();
    uint64_t sink = 0;
    for (int i = 0; i < REPS; i++) { bill_month(&t, r, n, &b, err, sizeof err); sink += b.total_paise; }
    double dt = now() - t0;
    double ns_per_reading = dt / ((double)REPS * (n - 1)) * 1e9;
    double ms_per_bill = dt / REPS * 1e3;
    printf("\nThroughput: %zu readings per bill, %.4f ms per monthly bill, %.2f ns per reading (checksum %" PRIu64 ")\n",
           n - 1, ms_per_bill, ns_per_reading, sink);
    double district_month_s = ms_per_bill * 800000.0 / 1e3;
    printf("One district (800,000 households), one month, one core : %.1f s (%.1f min)\n", district_month_s, district_month_s / 60);
    printf("One district, one YEAR (12 months, 35,040 readings each): %.1f min on one core\n", district_month_s * 12 / 60);
    printf("Memory for one household-month of readings: %zu readings x %zu bytes = %zu bytes (streamed one house at a time)\n",
           n, sizeof(uint32_t), n * sizeof(uint32_t));
    free(r);
    return 0;
}
