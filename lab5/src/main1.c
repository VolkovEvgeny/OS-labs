#include <stdio.h>
#include "lib.h"

int main(int argc, char const *argv[])
{
    while (!feof(stdin))
    {
        printf("\nWrite:\n [command] [arg1] ... [argN]\n");
        printf("\nIf you want to take integral of f(x) = sin(x), write 1 [A] [B] [C]\n");
        printf("\nIf you want to calculate number e (base of natural logarithm), write 2 [approximation]\n\n");

        int target;
        scanf("%d", &target);

        if (target == 1)
        {
            float a, b, e;
            scanf("%f %f %f", &a, &b, &e);
            printf("SinIntegral: %f\n", SinIntegral(a, b, e));
        }
        else if (target == 2)
        {
            int x;
            scanf("%d", &x);
            printf("E: %f\n", E(x));
        }
    }
    return 0;
}