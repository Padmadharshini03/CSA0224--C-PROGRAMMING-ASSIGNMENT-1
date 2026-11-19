#include <stdio.h>
#include <stdbool.h>

#define SLAB1_UNITS        100
#define SLAB2_UNITS        200
#define RATE1_PAISE        0L
#define RATE2_PAISE        225L
#define RATE3_PAISE        450L
#define FIXED_CHARGE_PAISE 5000L
#define MAX_METER_VAL      65535
#define MAX_REASONABLE_USE 10000U

typedef unsigned short meter_t;

typedef struct {
    unsigned int household_id;
    long prev_raw;
    long curr_raw;
    bool rollover_allowed;
} BillInput;

long calculate_bill_paise(unsigned int units)
{
    long paise = FIXED_CHARGE_PAISE;
    if (units > SLAB2_UNITS) {
        paise += (long)(units - SLAB2_UNITS) * RATE3_PAISE;
        paise += (long)(SLAB2_UNITS - SLAB1_UNITS) * RATE2_PAISE;
    } else if (units > SLAB1_UNITS) {
        paise += (long)(units - SLAB1_UNITS) * RATE2_PAISE;
    }
    return paise;
}

bool process_reading(BillInput in, unsigned int *out_units, long *out_paise, const char **err_msg)
{
    if (in.prev_raw < 0 || in.curr_raw < 0) {
        *err_msg = "REJECTED: Negative meter reading";
        return false;
    }
    if (in.prev_raw > MAX_METER_VAL || in.curr_raw > MAX_METER_VAL) {
        *err_msg = "REJECTED: Reading exceeds 16-bit max (65535)";
        return false;
    }
    if (in.curr_raw < in.prev_raw && !in.rollover_allowed) {
        *err_msg = "REJECTED: Current < Previous without rollover flag";
        return false;
    }

    unsigned int units = (meter_t)((meter_t)in.curr_raw - (meter_t)in.prev_raw);
    if (in.curr_raw < in.prev_raw && units > MAX_REASONABLE_USE) {
        *err_msg = "REJECTED: Implausible rollover jump";
        return false;
    }

    *out_units = units;
    *out_paise = calculate_bill_paise(units);
    *err_msg = "OK";
    return true;
}

int main(void)
{
    BillInput test_cases[] = {
        {101, 1000, 1000, false},  /* Boundary: 0 units -> Rs 50.00 */
        {102, 1000, 1060, false},  /* Slab 1: 60 units -> Rs 50.00 */
        {103, 1000, 1100, false},  /* Boundary: 100 units -> Rs 50.00 */
        {104, 1000, 1150, false},  /* Slab 2: 150 units -> Rs 162.50 */
        {105, 1000, 1200, false},  /* Boundary: 200 units -> Rs 275.00 */
        {106, 1000, 1350, false},  /* Slab 3: 350 units -> Rs 950.00 */
        {107, 65535, 0,   true},   /* Rollover 65535 -> 0: 1 unit -> Rs 50.00 */
        {108, 65400, 120, true},   /* Rollover 65400 -> 120: 256 units -> Rs 527.00 */
        {109, -10,   500, false},  /* Invalid: Negative reading */
        {110, 2000, 1800, false}   /* Invalid: Current < Previous without rollover */
    };

    int n = sizeof(test_cases) / sizeof(test_cases[0]);
    printf("ID  | Prev  | Curr  | Units | Bill (Rs) | Status\n");
    printf("----+-------+-------+-------+-----------+--------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        unsigned int units = 0;
        long paise = 0;
        const char *status = NULL;
        if (process_reading(test_cases[i], &units, &paise, &status)) {
            printf("%3u | %5ld | %5ld | %5u | %6ld.%02ld | %s\n",
                   test_cases[i].household_id, test_cases[i].prev_raw,
                   test_cases[i].curr_raw, units, paise / 100, paise % 100, status);
        } else {
            printf("%3u | %5ld | %5ld |   -   |     -     | %s\n",
                   test_cases[i].household_id, test_cases[i].prev_raw,
                   test_cases[i].curr_raw, status);
        }
    }

    /* 5-Bill Batch Verification (Hand-calculated total = Rs 1487.50) */
    BillInput batch5[] = {
        {201, 500, 500, false},  /* 0 u   -> Rs 50.00 */
        {202, 500, 600, false},  /* 100 u -> Rs 50.00 */
        {203, 500, 650, false},  /* 150 u -> Rs 162.50 */
        {204, 500, 700, false},  /* 200 u -> Rs 275.00 */
        {205, 500, 850, false}   /* 350 u -> Rs 950.00 */
    };
    long batch_total = 0;
    for (int i = 0; i < 5; i++) {
        unsigned int u;
        long p;
        const char *st;
        if (process_reading(batch5[i], &u, &p, &st)) {
            batch_total += p;
        }
    }
    printf("\n5-Bill Batch Total = Rs %ld.%02ld (Hand-calculated = Rs 1487.50)\n",
           batch_total / 100, batch_total % 100);

    return 0;
}
