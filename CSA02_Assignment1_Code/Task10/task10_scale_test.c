#include <stdio.h>
#include <limits.h>

int main(void)
{
    long scales[] = {4000L, 60000L, 800000L};
    const char *names[] = {"Village (4,000)", "Taluk (60,000)", "District (800,000)"};

    long bill_paise = 45535L;
    float bill_float = 455.35f;
    double bill_double = 455.35;

    printf("Scale              | Exact (long long)   | float Total         | Float Error (Rs) | int Overflow?\n");
    printf("-------------------+---------------------+---------------------+------------------+--------------\n");

    for (int s = 0; s < 3; s++) {
        long n = scales[s];
        long long exact_paise = 0LL;
        int int_paise = 0;
        float f_sum = 0.0f;
        double d_sum = 0.0;

        for (long i = 0; i < n; i++) {
            exact_paise += bill_paise;
            int_paise += (int)bill_paise;
            f_sum += bill_float;
            d_sum += bill_double;
        }

        double exact_rs = exact_paise / 100.0;
        printf("%-18s | Rs %14.2f | Rs %14.2f | %+16.2f | %s (%d)\n",
               names[s], exact_rs, f_sum, f_sum - exact_rs,
               (exact_paise > INT_MAX) ? "YES" : "NO", int_paise);
    }

    return 0;
}
