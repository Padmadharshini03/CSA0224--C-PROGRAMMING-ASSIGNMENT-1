/* Task 5 - program structure: header include, global declarations, main(), other functions.
 * Compile normal : gcc -Wall -Wextra billing_dbg.c -o billing_dbg
 * Compile debug  : gcc -Wall -Wextra -DDEBUG billing_dbg.c -o billing_dbg_debug             */
#include <stdio.h>
#include "tariff.h"                      /* our own header: searched in the current directory first */

static int bills_printed = 0;            /* global (file-scope) declaration */

int main(void)
{
    struct { meter_t prev, curr; } tests[] = { {1200, 1530}, {65400, 120}, {500, 500}, {0, 150} };
    int n = (int)(sizeof tests / sizeof tests[0]);

    for (int i = 0; i < n; i++) {
        unsigned int units = consumption(tests[i].prev, tests[i].curr);
        long total = total_bill(units);
        printf("prev=%5u curr=%5u units=%4u bill = Rs %ld.%02ld\n",
               (unsigned)tests[i].prev, (unsigned)tests[i].curr, units, total / 100, total % 100);
        bills_printed++;
    }
    printf("%d bills printed\n", bills_printed);
    return 0;
}

unsigned int consumption(meter_t previous, meter_t current)
{
    unsigned int u = (meter_t)(current - previous);
    DBG("consumption: prev=%u curr=%u -> units=%u\n", (unsigned)previous, (unsigned)current, u);
    return u;
}

long energy_charge(unsigned int units)
{
    long paise = 0;
    if (units > SLAB2_UNITS)
        paise += (long)(units - SLAB2_UNITS) * RATE3_PAISE;
    if (units > SLAB1_UNITS)
        paise += (long)((units > SLAB2_UNITS ? SLAB2_UNITS : units) - SLAB1_UNITS) * RATE2_PAISE;
    paise += (long)(units > SLAB1_UNITS ? SLAB1_UNITS : units) * RATE1_PAISE;
    DBG("energy_charge: units=%u -> %ld paise\n", units, paise);
    return paise;
}

long total_bill(unsigned int units)
{
    long e = energy_charge(units);
    long total = FIXED_CHARGE_PAISE + e;
    DBG("total_bill: fixed=%ld energy=%ld total=%ld\n", FIXED_CHARGE_PAISE, e, total);
    return total;
}
