/* Task 5 alternative: debugging controlled by a RUN-TIME variable instead of #ifdef DEBUG.
 * The test is executed on every call and the debug strings stay in the executable.
 * Run:  ./billing_rt        (quiet)       ./billing_rt -d     (debug on)                    */
#include <stdio.h>
#include <string.h>
#include "tariff.h"

static int debug = 0;                              /* run-time switch */
#define RDBG(...) do { if (debug) fprintf(stderr, "[DEBUG] " __VA_ARGS__); } while (0)

unsigned int consumption(meter_t previous, meter_t current)
{
    unsigned int u = (meter_t)(current - previous);
    RDBG("consumption: prev=%u curr=%u -> units=%u\n", (unsigned)previous, (unsigned)current, u);
    return u;
}
long energy_charge(unsigned int units)
{
    long paise = 0;
    if (units > SLAB2_UNITS) paise += (long)(units - SLAB2_UNITS) * RATE3_PAISE;
    if (units > SLAB1_UNITS) paise += (long)((units > SLAB2_UNITS ? SLAB2_UNITS : units) - SLAB1_UNITS) * RATE2_PAISE;
    RDBG("energy_charge: units=%u -> %ld paise\n", units, paise);
    return paise;
}
long total_bill(unsigned int units) { long e = energy_charge(units); RDBG("total_bill: energy=%ld\n", e); return FIXED_CHARGE_PAISE + e; }

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-d") == 0) debug = 1;
    unsigned int u = consumption(65400, 120);
    long t = total_bill(u);
    printf("units=%u bill = Rs %ld.%02ld\n", u, t / 100, t % 100);
    return 0;
}
