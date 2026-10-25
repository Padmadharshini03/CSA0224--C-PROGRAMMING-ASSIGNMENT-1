/* Task 12-B : the largest values that can now occur, and which C type is needed for each.
 * Assumptions (stated in the report):  max connected load 10 kW per household, 800,000 households (district),
 *   one reading every 15 minutes (96 per day, 35,040 per year), highest rate 450 paise/kWh, peak 120 %.
 * Compile: gcc -Wall -Wextra limits.c -o limits                                                            */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <inttypes.h>

typedef unsigned __int128 u128;

static int bits_needed(u128 v) { int b = 0; while (v) { b++; v >>= 1; } return b; }

static const char *smallest_type(u128 v)
{
    if (v <= UINT8_MAX)  return "uint8_t";
    if (v <= UINT16_MAX) return "uint16_t";
    if (v <= INT32_MAX)  return "int32_t (or uint32_t)";
    if (v <= UINT32_MAX) return "uint32_t";
    if (v <= INT64_MAX)  return "int64_t (or uint64_t)";
    if (v <= UINT64_MAX) return "uint64_t ONLY";
    return "128-bit (!)";
}

static void row(const char *what, u128 v, const char *note)
{
    char buf[48];
    if (v <= UINT64_MAX) snprintf(buf, sizeof buf, "%" PRIu64, (uint64_t)v);
    else snprintf(buf, sizeof buf, ">2^64");
    printf("%-46s %24s %5d  %-22s %s\n", what, buf, bits_needed(v), smallest_type(v), note);
}

int main(void)
{
    const uint64_t load_w = 10000, houses = 800000, per_day = 96, per_year = 35040;
    const uint64_t wh_interval = load_w / 4;                       /* 10 kW for 15 min = 2,500 Wh        */
    const uint64_t wh_month  = load_w * 24 * 31;                   /* 31-day month                       */
    const uint64_t wh_year   = load_w * 24 * 365;
    const uint64_t rate_max = 450, pct_max = 120;

    printf("Assumed: %" PRIu64 " W connected load, %" PRIu64 " households, %" PRIu64 " readings/day, %" PRIu64 " readings/year\n\n",
           load_w, houses, per_day, per_year);
    printf("%-46s %24s %5s  %-22s %s\n", "quantity (largest possible value)", "value", "bits", "smallest sufficient type", "effect on the ORIGINAL design");
    printf("%-46s %24s %5s  %-22s %s\n", "---------------------------------", "-----", "----", "------------------------", "---------------------");

    row("one reading (Wh in 15 min)",                  wh_interval,                          "fits meter_t (16-bit)");
    row("one household, one DAY (Wh)",                 load_w * 24,                          "meter_t 16-bit: OVERFLOWS");
    row("one household, one MONTH (Wh)",               wh_month,                             "int fits; meter_t OVERFLOWS");
    row("one household, one YEAR (Wh)",                wh_year,                              "int fits; meter_t OVERFLOWS");
    row("one household, one YEAR (kWh) [original unit]", wh_year / 1000,                      "meter_t 16-bit: OVERFLOWS (87,600 > 65,535)");
    row("cumulative register after 49 years at 10 kW", (u128)wh_year * 49,                   "fits uint32 register; wrap handled by unsigned subtraction");
    row("one DISTRICT, one MONTH (Wh)",                (u128)wh_month * houses,              "int and 32-bit long OVERFLOW");
    row("one DISTRICT, one YEAR (Wh)",                 (u128)wh_year * houses,               "int and 32-bit long OVERFLOW");
    row("readings per household-year (count)",         per_year,                             "unsigned short fits");
    row("readings per DISTRICT-year (count)",          (u128)per_year * houses,              "int OVERFLOWS");
    row("one reading's charge (Wh x rate x %)",        (u128)wh_interval * rate_max * pct_max, "fits a 32-bit long");
    row("household MONTH charge  Wh x rate x %",       (u128)wh_month * rate_max * pct_max,  "32-bit long OVERFLOWS");
    row("household YEAR charge   Wh x rate x %",       (u128)wh_year * rate_max * pct_max,   "32-bit long OVERFLOWS");
    row("DISTRICT YEAR charge    Wh x rate x % (1e-5 paise)", (u128)wh_year * houses * rate_max * pct_max, "fits int64_t with only ~2.4x headroom");
    row("one household YEAR bill, paise (rounded)",    ((u128)wh_year * rate_max * pct_max) / 100000ull, "fits int32; the SUM over households needs int64");
    return 0;
}
