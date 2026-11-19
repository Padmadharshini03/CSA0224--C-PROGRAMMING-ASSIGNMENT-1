#include <stdio.h>

int main(void)
{
    unsigned int house_id;
    int prev_reading, curr_reading;
    char category;

    printf("Enter Household ID (number): ");
    if (scanf("%u", &house_id) != 1) {
        printf("Error: Invalid Household ID (expected positive integer).\n");
        return 1;
    }

    printf("Enter Category (D = Domestic, C = Commercial): ");
    if (scanf(" %c", &category) != 1 || (category != 'D' && category != 'C')) {
        printf("Error: Category must be D or C.\n");
        return 1;
    }

    printf("Enter Previous and Current Meter Readings (0..65535): ");
    if (scanf("%d %d", &prev_reading, &curr_reading) != 2) {
        printf("Error: Non-numeric meter reading entered.\n");
        return 1;
    }

    if (prev_reading < 0 || prev_reading > 65535 || curr_reading < 0 || curr_reading > 65535) {
        printf("Error: Readings must be within 16-bit register range (0 to 65535).\n");
        return 1;
    }

    unsigned int units = (unsigned short)((unsigned short)curr_reading - (unsigned short)prev_reading);
    long total_paise = 5000L + (units > 100 ? (long)(units - 100) * 225L : 0L);

    printf("\n--- GRAM JYOTHI ELECTRICITY BILL ---\n");
    printf("Household ID   : %u\n", house_id);
    printf("Category       : %c\n", category);
    printf("Units Consumed : %u\n", units);
    printf("Total Payable  : Rs %ld.%02ld\n", total_paise / 100, total_paise % 100);

    return 0;
}
