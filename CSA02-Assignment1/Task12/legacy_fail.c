/* Task 12-B : what happens when the ORIGINAL design (Tasks 2/3/9) is fed smart-meter data.
 * Every line reproduces the original code exactly (16-bit meter_t, int/long, #define constants).
 * Compile: gcc -Wall -Wextra legacy_fail.c -o legacy_fail                                          */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

typedef unsigned short meter_t;                          /* ORIGINAL 16-bit register          */
#define SLAB1_UNITS 100
#define SLAB2_UNITS 200
#define RATE2_PAISE 225L
#define RATE3_PAISE 450L
#define FIXED_CHARGE_PAISE 5000L

static unsigned int consumption(meter_t previous, meter_t current) { return (meter_t)(current - previous); }
static long compute_bill(unsigned units)                  /* ORIGINAL: argument is in kWh      */
{
    long p = FIXED_CHARGE_PAISE;
    if (units > SLAB2_UNITS) p += (long)(units - SLAB2_UNITS) * RATE3_PAISE;
    if (units > SLAB1_UNITS) p += (long)((units > SLAB2_UNITS ? SLAB2_UNITS : units) - SLAB1_UNITS) * RATE2_PAISE;
    return p;
}

int main(void)
{
    printf("FAILURE 1 - watt-hours in a 16-bit register\n");
    uint32_t true_wh = 288000;                            /* the flat100 household: 288 kWh in a month */
    meter_t prev = 0, curr = (meter_t)(0 + true_wh);       /* register stores only the low 16 bits      */
    printf("  true consumption       : %u Wh\n", true_wh);
    printf("  consumption() returns  : %u Wh   (lost %u whole turns of the register)\n",
           consumption(prev, curr), (true_wh - consumption(prev, curr)) / 65536u);

    printf("\nFAILURE 2 - Wh passed where kWh is expected\n");
    printf("  compute_bill(288000 Wh treated as units) = Rs %ld.%02ld   (correct bill is Rs 692.24 with the peak rule)\n",
           compute_bill(288000u) / 100, compute_bill(288000u) % 100);

    printf("\nFAILURE 3 - 32-bit long (Windows / embedded) overflow in units x rate\n");
    int64_t year_wh = 87600000;                           /* 10 kW for a whole year in Wh        */
    int64_t exact = year_wh * 450 * 120;                  /* Wh x paise/kWh x percent (needs 43 bits) */
    int32_t wrapped = (int32_t)(uint32_t)((uint64_t)exact & 0xFFFFFFFFu);
    printf("  exact  year_wh x 450 x 120 = %lld   (needs %d bits)\n", (long long)exact, 64 - __builtin_clzll((unsigned long long)exact));
    printf("  as 32-bit signed           = %d   (WRONG)\n", wrapped);

    printf("\nFAILURE 4 - yearly total in kWh does not fit meter_t / unsigned short\n");
    unsigned year_kwh = 87600;
    printf("  10 kW for a year = %u kWh;  stored in meter_t = %u   (max %u)\n", year_kwh, (unsigned)(meter_t)year_kwh, (unsigned)USHRT_MAX);

    printf("\nFAILURE 5 - the tariff is fixed in the executable by #define\n");
    printf("  Changing the peak surcharge or a slab rate requires: edit source -> recompile -> redistribute.\n");
    printf("  RATE2_PAISE is baked in as the constant %ld; a time-of-day rule cannot be written with\n  #define at all, because the right rate depends on the reading's timestamp at run time.\n", (long)RATE2_PAISE);
    return 0;
}
