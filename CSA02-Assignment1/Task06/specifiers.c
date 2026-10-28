/* Task 6 - correct format specifier for every data type, and what a wrong one does.
 * Compile: gcc -Wall -Wextra specifiers.c -o specifiers                                  */
#include <stdio.h>
#include <limits.h>

int main(void)
{
    int            i  = -42;
    unsigned int   u  = 4000000000u;
    long           l  = 3000000000L;
    long long      ll = 9000000000000000000LL;
    float          f  = 162.5f;
    double         d  = 162.5;
    char           c  = 'D';
    const char    *s  = "Grama Jyothi";
    unsigned short us = 65535;

    printf("---- correct specifiers ----\n");
    printf("int            %%d   : %d\n", i);
    printf("unsigned int   %%u   : %u\n", u);
    printf("long           %%ld  : %ld\n", l);
    printf("long long      %%lld : %lld\n", ll);
    printf("unsigned short %%hu  : %hu\n", us);
    printf("float          %%f   : %f   (promoted to double by printf)\n", (double)f);
    printf("double         %%lf  : %lf  (%%f is also correct for printf)\n", d);
    printf("char           %%c   : %c\n", c);
    printf("string         %%s   : %s\n", s);
    printf("size_t         %%zu  : %zu\n", sizeof(long));

    printf("\n---- rupees and paise from an integer amount in paise ----\n");
    long amounts[] = { 16250L, 5000L, 5L, 86000L, 0L, 99L, 123456789L };
    for (size_t k = 0; k < sizeof amounts / sizeof amounts[0]; k++)
        printf("%10ld paise -> Rs %ld.%02ld\n", amounts[k], amounts[k] / 100, amounts[k] % 100);
    printf("\nwrong:   printf(\"Rs %%ld.%%ld\") for 5 paise -> Rs %ld.%ld   (missing leading zero)\n", 5L / 100, 5L % 100);
    printf("correct: printf(\"Rs %%ld.%%02ld\") for 5 paise -> Rs %ld.%02ld\n", 5L / 100, 5L % 100);
    return 0;
}
