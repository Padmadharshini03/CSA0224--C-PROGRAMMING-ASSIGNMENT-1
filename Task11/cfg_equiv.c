#define NO_MAIN
#include "billing_cfg.c"
static long compiled(unsigned u)
{
    long p = 5000L;
    if (u > 200) p += (long)(u - 200) * 450L;
    if (u > 100) p += (long)((u > 200 ? 200 : u) - 100) * 225L;
    return p;
}
int main(void)
{
    Tariff t; char err[100]; long bad = 0;
    if (load_tariff("tariff.cfg", &t, err, sizeof err)) { puts(err); return 1; }
    for (unsigned u = 0; u <= 65535; u++) if (bill_paise(&t, u) != compiled(u)) bad++;
    printf("equivalence over 65536 unit values: %ld mismatches\n", bad);
    return bad != 0;
}
