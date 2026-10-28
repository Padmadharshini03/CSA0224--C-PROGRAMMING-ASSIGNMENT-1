/* tariff.h - constants, types and prototypes for the Grama Jyothi billing program */
#ifndef TARIFF_H            /* include guard: the body is processed only once per translation unit */
#define TARIFF_H

#include <limits.h>

/* ---- types ---- */
typedef unsigned short meter_t;          /* 16-bit meter register, rolls over after 65535 */

/* ---- tariff constants (a tariff revision edits ONLY these lines) ---- */
#define SLAB1_UNITS        100           /* first 100 units                    */
#define SLAB2_UNITS        200           /* units 101 to 200                   */
#define RATE1_PAISE        0L            /* first slab is free                 */
#define RATE2_PAISE        225L          /* Rs 2.25 per unit                   */
#define RATE3_PAISE        450L          /* Rs 4.50 per unit above 200         */
#define FIXED_CHARGE_PAISE 5000L         /* Rs 50 fixed charge                 */
#define METER_MAX_READING  USHRT_MAX     /* 65535                              */

/* ---- debugging: DBG(...) prints only when compiled with  gcc -DDEBUG ---- */
#ifdef DEBUG
  #include <stdio.h>
  #define DBG(...) fprintf(stderr, "[DEBUG] " __VA_ARGS__)
#else
  #define DBG(...) ((void)0)             /* expands to nothing: no code, no strings */
#endif

/* ---- function prototypes ---- */
unsigned int consumption(meter_t previous, meter_t current);
long energy_charge(unsigned int units);
long total_bill(unsigned int units);

#endif /* TARIFF_H */
