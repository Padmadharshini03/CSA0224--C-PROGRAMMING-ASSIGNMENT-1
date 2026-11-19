#include <stdio.h>

int main(void)
{
    long amount_paise = 16250L;
    /* Intentionally mismatched specifier (%d instead of %ld) to trigger -Wformat */
    printf("Amount: %d\n", amount_paise);
    return 0;
}
