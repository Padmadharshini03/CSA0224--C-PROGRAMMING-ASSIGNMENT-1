/* Task 2 - Sizes and ranges of the candidate data types on this system,
 *          and a check of whether each is sufficient for a district.
 * Compile: gcc -Wall -Wextra datatypes.c -o datatypes
 */
#include <stdio.h>
#include <limits.h>
#include <float.h>

typedef unsigned short meter_t;

int main(void)
{
    printf("%-20s %5s  %-24s %-24s\n", "type", "bytes", "minimum", "maximum");
    printf("%-20s %5zu  %-24d %-24d\n", "char", sizeof(char), CHAR_MIN, CHAR_MAX);
    printf("%-20s %5zu  %-24d %-24d\n", "short", sizeof(short), SHRT_MIN, SHRT_MAX);
    printf("%-20s %5zu  %-24d %-24u\n", "unsigned short", sizeof(unsigned short), 0, (unsigned)USHRT_MAX);
    printf("%-20s %5zu  %-24d %-24d\n", "int", sizeof(int), INT_MIN, INT_MAX);
    printf("%-20s %5zu  %-24u %-24u\n", "unsigned int", sizeof(unsigned int), 0u, UINT_MAX);
    printf("%-20s %5zu  %-24ld %-24ld\n", "long", sizeof(long), LONG_MIN, LONG_MAX);
    printf("%-20s %5zu  %-24lu %-24lu\n", "unsigned long", sizeof(unsigned long), 0ul, ULONG_MAX);
    printf("%-20s %5zu  %-24lld %-24lld\n", "long long", sizeof(long long), LLONG_MIN, LLONG_MAX);
    printf("%-20s %5zu  %-24llu %-24llu\n", "unsigned long long", sizeof(unsigned long long), 0ull, ULLONG_MAX);
    printf("%-20s %5zu  %-24.6e %-24.6e   (FLT_DIG=%d, FLT_EPSILON=%g)\n", "float", sizeof(float),
           (double)FLT_MIN, (double)FLT_MAX, FLT_DIG, (double)FLT_EPSILON);
    printf("%-20s %5zu  %-24.6e %-24.6e   (DBL_DIG=%d, DBL_EPSILON=%g)\n", "double", sizeof(double),
           DBL_MIN, DBL_MAX, DBL_DIG, DBL_EPSILON);
    printf("%-20s %5zu\n", "meter_t (typedef)", sizeof(meter_t));

    printf("\n--- Is the type sufficient at district scale? ---\n");
    const long houses_district = 800000L;
    printf("households in a district = %ld\n", houses_district);
    printf("  fits unsigned short (max %u)? %s\n", (unsigned)USHRT_MAX,
           houses_district <= USHRT_MAX ? "yes" : "NO");
    printf("  fits int (max %d)?            %s\n", INT_MAX, houses_district <= INT_MAX ? "yes" : "NO");

    /* worst plausible bill: 10,000 units in a month */
    long long worst_bill = 5000LL + 100LL * 225 + 9800LL * 450;
    long long district_total = worst_bill * houses_district;
    printf("worst plausible bill (10000 units) = %lld paise\n", worst_bill);
    printf("district total if every house had it = %lld paise\n", district_total);
    printf("  fits int (max %d)?            %s\n", INT_MAX, district_total <= INT_MAX ? "yes" : "NO");
    printf("  fits 32-bit long (max 2147483647)? %s\n", district_total <= 2147483647LL ? "yes" : "NO");
    printf("  fits long long (max %lld)? %s\n", LLONG_MAX, district_total <= LLONG_MAX ? "yes" : "NO");

    printf("\n--- 16-bit unsigned register after 65535 ---\n");
    meter_t m = 65535;
    printf("m = %u\n", (unsigned)m);
    m = m + 1;                       /* the int result 65536 is converted back to 16 bits */
    printf("m + 1 stored back in meter_t = %u  (wrapped to 0, never negative)\n", (unsigned)m);
    return 0;
}
