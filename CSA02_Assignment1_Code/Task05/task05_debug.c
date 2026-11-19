#include <stdio.h>
#include "billing.h"

unsigned int calc_units(meter_t prev, meter_t curr)
{
    unsigned int units = (meter_t)(curr - prev);
#ifdef DEBUG
    printf("[DEBUG] prev=%u, curr=%u -> units=%u\n", prev, curr, units);
#endif
    return units;
}

long calc_bill_paise(unsigned int units)
{
    long energy = 0L;
    if (units > SLAB2_UNITS) {
        energy = (long)(units - SLAB2_UNITS) * RATE3_PAISE
               + (long)(SLAB2_UNITS - SLAB1_UNITS) * RATE2_PAISE;
    } else if (units > SLAB1_UNITS) {
        energy = (long)(units - SLAB1_UNITS) * RATE2_PAISE;
    }

    long total = FIXED_CHARGE_PAISE + energy;
#ifdef DEBUG
    printf("[DEBUG] units=%u -> fixed=%ld, energy=%ld, total=%ld paise\n",
           units, FIXED_CHARGE_PAISE, energy, total);
#endif
    return total;
}

int main(void)
{
    unsigned int u = calc_units(65400, 120);
    long bill = calc_bill_paise(u);
    printf("Final Bill: Rs %ld.%02ld\n", bill / 100, bill % 100);
    return 0;
}
