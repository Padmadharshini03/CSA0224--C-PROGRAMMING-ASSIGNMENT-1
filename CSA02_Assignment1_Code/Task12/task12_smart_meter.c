#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t slab1_wh;          /* 100 kWh = 100,000 Wh */
    uint64_t slab2_wh;          /* 200 kWh = 200,000 Wh */
    int64_t rate1_paise_kwh;    /* 0 paise/kWh */
    int64_t rate2_paise_kwh;    /* 225 paise/kWh */
    int64_t rate3_paise_kwh;    /* 450 paise/kWh */
    int64_t fixed_charge_paise; /* 5000 paise */
    int peak_surcharge_pct;     /* 20 (%) between 18:00 and 22:00 */
} TariffConfig;

bool is_peak_slot(int hour_24)
{
    return (hour_24 >= 18 && hour_24 < 22);
}

int64_t compute_smart_bill_paise(uint64_t offpeak_wh, uint64_t peak_wh, const TariffConfig *cfg)
{
    uint64_t weighted_peak_wh = (peak_wh * (100 + cfg->peak_surcharge_pct)) / 100ULL;
    uint64_t effective_wh = offpeak_wh + weighted_peak_wh;

    int64_t energy_paise = 0;
    if (effective_wh > cfg->slab2_wh) {
        uint64_t s3_wh = effective_wh - cfg->slab2_wh;
        uint64_t s2_wh = cfg->slab2_wh - cfg->slab1_wh;
        energy_paise += (int64_t)((s3_wh * (uint64_t)cfg->rate3_paise_kwh + 500ULL) / 1000ULL);
        energy_paise += (int64_t)((s2_wh * (uint64_t)cfg->rate2_paise_kwh + 500ULL) / 1000ULL);
    } else if (effective_wh > cfg->slab1_wh) {
        uint64_t s2_wh = effective_wh - cfg->slab1_wh;
        energy_paise += (int64_t)((s2_wh * (uint64_t)cfg->rate2_paise_kwh + 500ULL) / 1000ULL);
    }

    return cfg->fixed_charge_paise + energy_paise;
}

int main(void)
{
    TariffConfig cfg = {
        .slab1_wh = 100000ULL,
        .slab2_wh = 200000ULL,
        .rate1_paise_kwh = 0,
        .rate2_paise_kwh = 225,
        .rate3_paise_kwh = 450,
        .fixed_charge_paise = 5000,
        .peak_surcharge_pct = 20
    };

    uint64_t offpeak_wh = 180000ULL;
    uint64_t peak_wh = 50000ULL;

    int64_t bill = compute_smart_bill_paise(offpeak_wh, peak_wh, &cfg);
    printf("Off-Peak Consumption : %llu Wh (%.2f kWh)\n",
           (unsigned long long)offpeak_wh, offpeak_wh / 1000.0);
    printf("Peak (6PM-10PM)      : %llu Wh (%.2f kWh, +20%% effective = %.2f kWh)\n",
           (unsigned long long)peak_wh, peak_wh / 1000.0, (peak_wh * 1.2) / 1000.0);
    printf("Total Smart Bill     : Rs %lld.%02lld\n",
           (long long)(bill / 100), (long long)(bill % 100));

    uint64_t max_district_yearly_paise = 800000ULL * 12ULL * (uint64_t)bill;
    printf("Projected District Yearly Revenue (800k houses): Rs %llu.%02llu (fits easily in uint64_t)\n",
           (unsigned long long)(max_district_yearly_paise / 100ULL),
           (unsigned long long)(max_district_yearly_paise % 100ULL));

    return 0;
}
