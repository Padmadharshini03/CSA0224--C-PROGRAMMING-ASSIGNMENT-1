/* smart_bill - bill one household for one month from 15-minute smart-meter readings.
 * Compile: gcc -Wall -Wextra -O2 smart_main.c smart.c -o smart_bill
 * Run    : ./smart_bill tariff_smart.cfg readings.txt
 * Exit   : 0 bill printed, 1 usage, 2 tariff error, 3 readings error, 4 billing error        */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "smart.h"

static void money(const char *label, uint64_t paise)
{
    printf("  %-34s Rs %" PRIu64 ".%02" PRIu64 "\n", label, paise / 100, paise % 100);
}
static void kwh(const char *label, uint64_t wh)
{
    printf("  %-34s %" PRIu64 ".%03" PRIu64 " kWh\n", label, wh / 1000, wh % 1000);
}

int main(int argc, char **argv)
{
    if (argc != 3) { fprintf(stderr, "usage: %s tariff.cfg readings.txt\n", argv[0]); return 1; }
    char err[200];
    Tariff t;
    if (tariff_load(argv[1], &t, err, sizeof err)) { fprintf(stderr, "TARIFF ERROR: %s\n", err); return 2; }
    uint32_t *reg; size_t n;
    if (readings_load(argv[2], &reg, &n, err, sizeof err)) { fprintf(stderr, "READINGS ERROR: %s\n", err); return 3; }
    MonthBill b;
    if (bill_month(&t, reg, n, &b, err, sizeof err)) { fprintf(stderr, "BILLING ERROR: %s\n", err); free(reg); return 4; }

    printf("Billing period: %" PRIu64 " readings of %u minutes (%.2f days)\n", b.intervals, t.interval_min,
           (double)b.intervals * t.interval_min / 1440.0);
    kwh("Energy consumed", b.total_wh);
    kwh("  of which peak", b.peak_wh);
    kwh("  of which off-peak", b.offpeak_wh);
    for (int k = 0; k < t.nslabs; k++) {
        char lab[64];
        snprintf(lab, sizeof lab, "  slab %d @ %u paise/kWh", k + 1, t.slab_rate[k]);
        kwh(lab, b.slab_wh[k]);
    }
    money("Energy charge (normal rates)", b.base_paise);
    money("Peak-hour surcharge", b.surcharge_paise);
    money("Energy charge total", b.energy_paise);
    money("Fixed charge", b.fixed_paise);
    money("TOTAL BILL", b.total_paise);
    printf("TOTAL_PAISE=%" PRIu64 "\n", b.total_paise);
    free(reg);
    return 0;
}
