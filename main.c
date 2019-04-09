#include <stdio.h>

int main() {


    double a = 7.0;

    unsigned long long * p;

    for (int y = 0; y < 25; y++) {
        a /= 2;
        p = (unsigned long long *) &a;
        printf("\n%.20lf\t", a);

        for (unsigned int k = 8 * sizeof(double) - 1; k >= 0; k--) {
            printf("%llu", ((*p) >> k) & 0x01);
            if ((k) % 8 == 0)
                printf(" ");
        }
    }


    return 0;
}