#include <stdio.h>
#include <limits.h>

int main(void)
{
    /* Surprise 1: Integer division truncation */
    int units = 150;
    double wrong_bill = 50.0 + (units / 100) * 225.0;
    double right_bill = 50.0 + (units / 100.0) * 225.0;
    printf("Surprise 1 (Integer Division): wrong = %.2f vs right = %.2f\n", wrong_bill, right_bill);

    /* Surprise 2: Signed-to-unsigned usual arithmetic conversion */
    int delta = -50;
    unsigned int threshold = 100U;
    if (delta > threshold) {
        printf("Surprise 2 (Signed/Unsigned): -50 > 100U is TRUE because -50 converts to %uU!\n",
               (unsigned int)delta);
    }

    /* Surprise 3: Narrowing cast from 64-bit long to 32-bit int */
    long district_paise = 3000000000L;
    int truncated_paise = (int)district_paise;
    printf("Surprise 3 (Narrowing Cast): long %ld -> int %d (overflow!)\n",
           district_paise, truncated_paise);

    /* Consistent Rounding Rule: Half-up rounding to nearest paisa */
    double tax_rupees = 18.456;
    long rounded_paise = (long)(tax_rupees * 100.0 + 0.5);
    printf("Half-up Rounding: Rs %.3f -> %ld paise (Rs %ld.%02ld)\n",
           tax_rupees, rounded_paise, rounded_paise / 100, rounded_paise % 100);

    return 0;
}
