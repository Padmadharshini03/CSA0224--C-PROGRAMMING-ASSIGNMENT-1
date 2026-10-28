#include "billing.h"

unsigned int consumption(meter_t previous, meter_t current)
{
    return (meter_t)(current - previous);
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

long total_bill(unsigned int units) { return FIXED_CHARGE_PAISE + energy_charge(units); }

Bill make_bill(const Record *r)
{
    Bill b;
    b.units  = consumption(r->previous, r->current);
    b.energy = energy_charge(b.units);
    b.fixed  = FIXED_CHARGE_PAISE;
    b.total  = b.fixed + b.energy;
    return b;
}
