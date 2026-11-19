#ifndef BILLING_H
#define BILLING_H

#include <stdint.h>

#define SLAB1_UNITS        100
#define SLAB2_UNITS        200
#define RATE1_PAISE        0L
#define RATE2_PAISE        225L
#define RATE3_PAISE        450L
#define FIXED_CHARGE_PAISE 5000L

typedef unsigned short meter_t;

unsigned int calc_units(meter_t prev, meter_t curr);
long calc_bill_paise(unsigned int units);

#endif /* BILLING_H */
