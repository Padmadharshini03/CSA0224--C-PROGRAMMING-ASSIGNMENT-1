/* Task 6: time to read+convert N lines with scanf versus fgets+strtol.  Input file made by gen below. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec*1e-9; }
int main(int argc, char **argv)
{
    if (argc < 3) return 1;
    int mode = atoi(argv[1]);           /* 1 = scanf, 2 = fgets+strtol */
    FILE *fp = fopen(argv[2], "r");
    if (!fp) return 1;
    long h, p, c, sum = 0, n = 0;
    double t0 = now();
    if (mode == 1) {
        while (fscanf(fp, "%ld %ld %ld", &h, &p, &c) == 3) { sum += c - p; n++; }
    } else {
        char buf[256];
        while (fgets(buf, sizeof buf, fp)) {
            char *e; h = strtol(buf, &e, 10); p = strtol(e, &e, 10); c = strtol(e, &e, 10);
            sum += c - p; n++;
        }
    }
    double t1 = now();
    printf("%-14s %8ld lines  %7.1f ms  %6.1f ns/line  (checksum %ld)\n",
           mode == 1 ? "scanf" : "fgets+strtol", n, (t1 - t0) * 1e3, (t1 - t0) / (double)n * 1e9, sum);
    fclose(fp);
    return 0;
}
