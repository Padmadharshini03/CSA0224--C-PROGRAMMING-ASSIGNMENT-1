#include "bad_guard.h"
#include "bad_guard.h"      /* second inclusion -> redefinition of struct Tariff */
int main(void) { struct Tariff t = {1, 2}; return (int)t.fixed - 1; }
