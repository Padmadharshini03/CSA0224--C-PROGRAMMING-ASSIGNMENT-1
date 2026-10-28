/* smart.h - redesigned billing core for smart meters (Task 12)
 *
 * Changes from the original design (see report):
 *   readings   : cumulative register in WATT-HOURS, uint32_t, one per 15 minutes
 *   arithmetic : exact 64-bit integers; rates in paise per kWh; charge kept in 1e-5 paise
 *                ("micro-units") and rounded ONCE per bill, half up
 *   tariff     : read from a configuration file at start-up (no #define for values)
 *   time rule  : units used in [peak_start, peak_end) cost peak_percent % of the normal rate
 */
#ifndef SMART_H
#define SMART_H

#include <stdint.h>
#include <stddef.h>

#define SMART_MAX_SLABS   16
#define SMART_UNLIMITED   UINT64_MAX
#define MINUTES_PER_DAY   1440u
#define WH_PER_KWH        1000u
#define PERCENT_BASE      100u
/* charge unit: Wh x (paise/kWh) x percent  =  1/(1000*100) paise = 1e-5 paise */
#define MICRO_PER_PAISE   100000ull

typedef struct {
    uint32_t fixed_paise;                       /* fixed charge per bill                      */
    uint32_t interval_min;                      /* minutes between readings (15)              */
    uint32_t peak_start_min, peak_end_min;      /* peak window [start,end) in minutes of day  */
    uint32_t peak_percent;                      /* 120 = peak units cost 20% more             */
    uint32_t max_interval_wh;                   /* plausibility limit for one reading         */
    int      nslabs;
    uint64_t slab_limit_wh[SMART_MAX_SLABS];    /* cumulative monthly limit in Wh             */
    uint32_t slab_rate[SMART_MAX_SLABS];        /* paise per kWh                              */
} Tariff;

typedef struct {
    uint64_t intervals;
    uint64_t total_wh, peak_wh, offpeak_wh;
    uint64_t slab_wh[SMART_MAX_SLABS];
    uint64_t base_u, total_u;                   /* energy charge in 1e-5 paise (without / with peak) */
    uint64_t energy_paise, base_paise, surcharge_paise, fixed_paise, total_paise;
} MonthBill;

/* 0 = ok, otherwise < 0 and a message in err */
int  tariff_load(const char *path, Tariff *t, char *err, size_t errn);
int  bill_month(const Tariff *t, const uint32_t *reg, size_t nreg, MonthBill *out, char *err, size_t errn);
int  readings_load(const char *path, uint32_t **reg, size_t *n, char *err, size_t errn);
int  is_peak_slot(const Tariff *t, uint64_t slot_index);
uint64_t round_micro_to_paise(uint64_t micro);

#endif
