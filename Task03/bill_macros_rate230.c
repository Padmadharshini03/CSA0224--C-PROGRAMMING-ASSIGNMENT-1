#include <stdio.h>
#define SLAB1_UNITS 100 /* first 100 units */
#define SLAB2_UNITS 200 /* units 101 to 200 */
#define RATE1_PAISE 0 /* first slab is free */
#define RATE2_PAISE 230 /* Rs 2.25 per unit */
#define RATE3_PAISE 450 /* Rs 4.50 per unit above 200 */
#define FIXED_CHARGE_PAISE 5000L /* Rs 50 fixed charge */
/* Returns the bill in paise, so that no rounding error can occur. */
long compute_bill(int units)
{
 long paise = FIXED_CHARGE_PAISE;
 // TODO 1: Add the energy charge slab by slab using the macros: units above
 // SLAB2_UNITS at RATE3_PAISE, units from SLAB1_UNITS+1 to SLAB2_UNITS at
 // RATE2_PAISE, and the rest at RATE1_PAISE
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
