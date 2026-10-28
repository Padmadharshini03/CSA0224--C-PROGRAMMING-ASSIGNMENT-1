/* Task 2 - Why money must not be a float: adding n identical bills.
 */
#include <stdio.h>

int main(void)
{
    const int sizes[] = { 4000, 60000, 800000 };
    const float  bill_f = 162.35f;      /* float rupees  (alternative 1) */
    const double bill_d = 162.35;       /* double rupees (alternative 2) */
    const long   bill_p = 16235L;       /* integer paise (chosen design) */

    printf("Each bill = Rs 162.35.  Exact total = n x 16235 paise.\n\n");
    printf("%8s | %-16s %-12s | %-16s %-12s | %-14s\n",
           "n", "float total (Rs)", "error (Rs)", "double total", "error (Rs)", "paise total");

    for (size_t k = 0; k < sizeof sizes / sizeof sizes[0]; k++) {
        int n = sizes[k];
        float  sf = 0.0f;
        double sd = 0.0;
        long long sp = 0;
        for (int i = 0; i < n; i++) {
            sf += bill_f;
            sd += bill_d;
            sp += bill_p;
        }
        double exact_rs = (double)sp / 100.0;
        printf("%8d | %16.4f %12.4f | %16.6f %12.9f | Rs %lld.%02lld\n",
               n, (double)sf, (double)sf - exact_rs, sd, sd - exact_rs,
               sp / 100, sp % 100);
    }
    return 0;
}
