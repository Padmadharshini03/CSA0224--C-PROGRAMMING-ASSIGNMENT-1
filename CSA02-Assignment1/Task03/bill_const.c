/* Task 3 - Alternative concept: the same tariff written as enum + const variables
 * instead of #define.  Output is identical to bill_macros.c.
 * Compile: gcc -Wall -Wextra -O0 -c bill_const.c   then   nm bill_const.o
 */
#include <stdio.h>

enum { SLAB1_UNITS = 100, SLAB2_UNITS = 200 };      /* integer constants: no storage   */
const long RATE1_PAISE = 0L;                        /* typed, have an address, scope   */
const long RATE2_PAISE = 225L;
const long RATE3_PAISE = 450L;
const long FIXED_CHARGE_PAISE = 5000L;

long compute_bill(int units)
{
    long paise = FIXED_CHARGE_PAISE;
    if (units > SLAB2_UNITS)
        paise += (long)(units - SLAB2_UNITS) * RATE3_PAISE;
    if (units > SLAB1_UNITS)
        paise += (long)((units > SLAB2_UNITS ? SLAB2_UNITS : units) - SLAB1_UNITS) * RATE2_PAISE;
    paise += (long)(units > SLAB1_UNITS ? SLAB1_UNITS : units) * RATE1_PAISE;
    return paise;
}

int main(void)
{
    int tests[] = {0, 100, 150, 200, 350};
    int n = sizeof tests / sizeof tests[0];
    for (int i = 0; i < n; i++) {
        long p = compute_bill(tests[i]);
        printf("%3d units -> Rs %ld.%02ld\n", tests[i], p / 100, p % 100);
    }
    return 0;
}
