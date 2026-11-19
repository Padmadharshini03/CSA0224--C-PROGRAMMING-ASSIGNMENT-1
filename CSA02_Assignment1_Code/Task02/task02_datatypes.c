#include <stdio.h>
#include <limits.h>
#include <float.h>

/* The meter register is a 16-bit counter: after 65535 it rolls over to 0. */
typedef unsigned short meter_t;

unsigned int consumption(meter_t previous, meter_t current)
{
    /* Unsigned 16-bit subtraction wraps modulo 65536 automatically without if-else */
    return (meter_t)(current - previous);
}

int main(void)
{
    /* 1. Required Complete-the-Code Verification */
    printf("sizeof(meter_t) = %zu bytes, maximum reading = %u\n",
           sizeof(meter_t), (unsigned)USHRT_MAX);
    printf("Consumption: %u units\n", consumption(1200, 1530));
    printf("Consumption: %u units\n\n", consumption(65400, 120)); /* rolled over */

    /* 2. System Data Type Sizes & Ranges */
    printf("--- Data Type Sizes and Ranges ---\n");
    printf("unsigned short : %zu bytes | 0 to %u\n", sizeof(unsigned short), USHRT_MAX);
    printf("int            : %zu bytes | %d to %d\n", sizeof(int), INT_MIN, INT_MAX);
    printf("unsigned int   : %zu bytes | 0 to %u\n", sizeof(unsigned int), UINT_MAX);
    printf("long           : %zu bytes | %ld to %ld\n", sizeof(long), LONG_MIN, LONG_MAX);
    printf("long long      : %zu bytes | %lld to %lld\n", sizeof(long long), LLONG_MIN, LLONG_MAX);
    printf("float          : %zu bytes | %e to %e (FLT_DIG = %d)\n",
           sizeof(float), FLT_MIN, FLT_MAX, FLT_DIG);
    printf("double         : %zu bytes | %e to %e (DBL_DIG = %d)\n\n",
           sizeof(double), DBL_MIN, DBL_MAX, DBL_DIG);

    /* 3. Float Precision Drift vs. Exact Integer Paise over 4,000 Bills */
    float float_total = 0.0f;
    long paise_total = 0L;
    float bill_rupees = 162.55f;
    long bill_paise = 16255L;

    for (int i = 0; i < 4000; i++) {
        float_total += bill_rupees;
        paise_total += bill_paise;
    }

    printf("--- 4,000 Bills Precision Comparison (Rs 162.55 each) ---\n");
    printf("Expected Exact Total : Rs %ld.%02ld\n", paise_total / 100, paise_total % 100);
    printf("Float Accumulated    : Rs %.2f (raw: %.6f)\n", float_total, float_total);
    printf("Discrepancy          : Rs %.4f\n", float_total - (paise_total / 100.0));

    return 0;
}
