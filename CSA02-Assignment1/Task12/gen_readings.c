/* gen_readings - write a month of cumulative register readings (Wh), one per 15 minutes.
 * usage: gen_readings scenario days [opening_register]
 *   scenarios:  flat100        every interval uses exactly 100 Wh
 *               total:<Wh>     <Wh> spread as evenly as possible over the month
 *               profile        a daily load shape (night low, morning, evening peak) with pseudo-random variation
 *               peakonly       all consumption inside the 18:00-22:00 window (400 Wh per peak interval)
 *               fault          flat100 with one register that jumps backwards (meter fault)
 *               heavy          10 kW continuous (2500 Wh per interval): the largest physically possible load  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: %s scenario days [opening_register]\n", argv[0]); return 1; }
    const char *sc = argv[1];
    unsigned days = (unsigned)atoi(argv[2]);
    uint32_t reg = argc > 3 ? (uint32_t)strtoull(argv[3], NULL, 10) : 0u;
    unsigned slots = days * 96u;
    unsigned long long total = 0;
    int is_total = 0;
    if (strncmp(sc, "total:", 6) == 0) { total = strtoull(sc + 6, NULL, 10); is_total = 1; }
    uint64_t seed = 42;

    printf("# scenario=%s days=%u opening register first\n", sc, days);
    printf("%" PRIu32 "\n", reg);
    for (unsigned i = 0; i < slots; i++) {
        unsigned slot_of_day = i % 96u;
        unsigned hour = slot_of_day / 4u;
        uint32_t wh = 0;
        if (strcmp(sc, "flat100") == 0 || strcmp(sc, "fault") == 0) wh = 100;
        else if (is_total) wh = (uint32_t)(total / slots + (i < total % slots ? 1 : 0));
        else if (strcmp(sc, "peakonly") == 0) wh = (hour >= 18 && hour < 22) ? 400 : 0;
        else if (strcmp(sc, "heavy") == 0) wh = 2500;
        else if (strcmp(sc, "profile") == 0) {
            seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
            unsigned noise = (unsigned)((seed >> 33) % 21);                 /* 0..20 */
            unsigned base = (hour < 5) ? 20 : (hour < 9) ? 90 : (hour < 17) ? 45 : (hour < 22) ? 160 : 60;
            wh = base + noise;
        } else { fprintf(stderr, "unknown scenario\n"); return 1; }
        reg += wh;                                   /* uint32_t wraps by itself at 2^32 */
        if (strcmp(sc, "fault") == 0 && i == 1000) reg -= 5000u;       /* register jumps backwards */
        printf("%" PRIu32 "\n", reg);
    }
    return 0;
}
