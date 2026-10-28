/* Task 7 - where different types are combined, and the surprises that result.
 * Compile: gcc -Wall -Wextra conversions.c -o conversions
 * (some lines are expected to produce warnings; they are the point of the demonstration)    */
#include <stdio.h>
#include <limits.h>
#include <math.h>

typedef unsigned short meter_t;

/* the rounding rule used by the whole project: round half up, on integers only */
static long round_div(long numerator, long denominator)       /* numerator >= 0 */
{
    return (numerator + denominator / 2) / denominator;
}

int main(void)
{
    printf("===== THREE (+) EXPRESSIONS THAT GIVE AN UNEXPECTED RESULT =====\n");

    /* 1. integer division */
    int units = 150;
    double wrong1 = units / 100 * 2.25;               /* 150/100 is int division = 1 */
    double right1 = units / 100.0 * 2.25;
    printf("1) units/100*2.25      with units=150 : %.4f   (intended 3.3750)\n", wrong1);
    printf("   units/100.0*2.25                   : %.4f\n", right1);

    /* 2. explicit cast of a large long to int */
    long big = 3000000000L;
    int  truncated = (int)big;
    printf("2) (int)3000000000L                   : %d   (intended 3000000000)\n", truncated);

    /* 3. signed/unsigned comparison */
    int minus_one = -1;
    unsigned int one = 1u;
    printf("3) (-1 < 1u)                          : %d   (intended 1: -1 is converted to 4294967295u)\n",
           minus_one < one);                           /* -Wsign-compare warns here */

    /* 4. float cannot hold every integer */
    float f = 16777217.0f;
    printf("4) (float)16777217                    : %.1f   (intended 16777217.0)\n", (double)f);

    /* 5. 0.1+0.2 */
    double s = 0.1 + 0.2;
    printf("5) 0.1 + 0.2 == 0.3                   : %d   (sum prints as %.17g)\n", s == 0.3, s);

    /* 6. truncation instead of rounding: 7 units at the revised rate Rs 2.30 -> paise */
    double rupees = 7 * 2.30;                         /* 7 units x Rs 2.30 = Rs 16.10 */
    long truncated_paise = (long)(rupees * 100);
    long rounded_paise   = lround(rupees * 100);
    printf("6) (long)(7*2.30*100)                 : %ld   (intended 1610; the double is %.17g)\n",
           truncated_paise, rupees * 100);
    printf("   lround(7*2.30*100)                 : %ld\n", rounded_paise);

    /* 7. 16-bit register: integer promotion hides the roll-over */
    meter_t m = 65535;
    int promoted = m + 1;                             /* m is promoted to int, result 65536 */
    meter_t stored = (meter_t)(m + 1);
    printf("7) meter_t 65535 + 1 (promoted to int): %d;  stored back in meter_t : %u\n", promoted, (unsigned)stored);

    /* 8. int overflow in multiplication */
    volatile int a = 50000;
    long long safe = (long long)a * a;
    printf("8) 50000*50000 as int (wraps)         : %d;  with (long long) cast : %lld\n",
           (int)((unsigned)a * (unsigned)a), safe);

    printf("\n===== ROUNDING RULE: round half up on integers =====\n");
    long charge = 233;                                /* paise per unit before a 20%% peak surcharge */
    printf("233 paise x 120%% = %.1f -> round_div(233*120, 100) = %ld paise\n",
           233 * 1.20, round_div(charge * 120, 100));
    printf("225 paise x 120%% = %.1f -> round_div(225*120, 100) = %ld paise\n",
           225 * 1.20, round_div(225L * 120, 100));
    printf("round_div(5,10)=%ld  round_div(4,10)=%ld  round_div(15,10)=%ld  round_div(14,10)=%ld\n",
           round_div(5, 10), round_div(4, 10), round_div(15, 10), round_div(14, 10));

    printf("\n===== IMPLICIT CONVERSION POINTS in the billing program =====\n");
    printf("unsigned short -> int  (promotion)       : current - previous in consumption()\n");
    printf("int -> unsigned int    (return value)    : units\n");
    printf("unsigned int -> long   (cast in product) : (long)(units - SLAB2_UNITS) * RATE3_PAISE\n");
    printf("long -> long long      (totals)          : total_paise += bill\n");
    printf("long -> int            (%%d specifier)    : the ONLY unsafe one, avoided by using %%ld\n");
    return 0;
}
