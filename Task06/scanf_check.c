/* Task 6 - read with scanf and validate with its return value.
 * Input line:  household  previous  current          e.g.   101 1200 1530
 * Compile: gcc -Wall -Wextra scanf_check.c -o scanf_check
 * Run    : ./scanf_check < input_valid.txt        ./scanf_check < input_invalid.txt          */
#include <stdio.h>
#include "tariff.h"

static void discard_line(void)                     /* remove the rest of a bad line */
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

unsigned int consumption(meter_t previous, meter_t current) { return (meter_t)(current - previous); }
long energy_charge(unsigned int units)
{
    long p = 0;
    if (units > SLAB2_UNITS) p += (long)(units - SLAB2_UNITS) * RATE3_PAISE;
    if (units > SLAB1_UNITS) p += (long)((units > SLAB2_UNITS ? SLAB2_UNITS : units) - SLAB1_UNITS) * RATE2_PAISE;
    return p + (long)(units > SLAB1_UNITS ? SLAB1_UNITS : units) * RATE1_PAISE;
}
long total_bill(unsigned int units) { return FIXED_CHARGE_PAISE + energy_charge(units); }

int main(void)
{
    long house, prev, curr;
    int line = 0, ok = 0, bad = 0;

    for (;;) {
        line++;
        int n = scanf("%ld %ld %ld", &house, &prev, &curr);     /* returns number of items read */
        if (n == EOF)
            break;
        if (n != 3) {                                           /* letters or a missing field   */
            printf("line %d: INVALID INPUT (scanf returned %d, expected 3) - line skipped\n", line, n);
            discard_line();
            bad++;
            continue;
        }
        discard_line();                                         /* stay in step with the input lines */
        if (prev < 0 || prev > METER_MAX_READING || curr < 0 || curr > METER_MAX_READING) {
            printf("line %d: reading out of range 0..%d - line skipped\n", line, METER_MAX_READING);
            bad++;
            continue;
        }
        unsigned int u = consumption((meter_t)prev, (meter_t)curr);
        long t = total_bill(u);
        printf("line %d: house %ld units %u -> Rs %ld.%02ld\n", line, house, u, t / 100, t % 100);
        ok++;
    }
    printf("\n%d valid, %d rejected\n", ok, bad);
    return 0;
}
