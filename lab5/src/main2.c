#include <dlfcn.h>
#include <stdio.h>

const char *names[] = {"./liblib1.so", "./liblib2.so"};

int main()
{
    int n = 0;
    void *handle;
    float (*E)(int);
    float (*SinIntegral)(float, float, float);
    handle = dlopen(names[n], RTLD_LAZY); // dlopen загружает боблиотеку в оперативную память
    E = dlsym(handle, "E"); // dlsym определяет функцию из определенной библиотеки 
    SinIntegral = dlsym(handle, "SinIntegral");
    int t;
    while (!feof(stdin))
    {
        printf("\nWrite:\n [command] [arg1] ... [argN]\n");
        printf("\nIf you want to change methods of calculation, write 0\n");
        printf("\nIf you want to take integral of f(x) = sin(x), write 1 [A] [B] [C]\n");
        printf("\nIf you want to calculate number e (base of natural logarithm), write 2 [approximation]\n\n");
        printf("Current lib is %d\n\n", n);
        
        scanf("%d", &t);

        if (t == 0)
        {
            n = (n + 1) % 2; // меняет 0 на 1, 1 на 0

            if (dlclose(handle) != 0)   // dlclose освобождает память от библиотеки
            {
                perror("dlclose error");
                return -1;
            };

            handle = dlopen(names[n], RTLD_LAZY);  // загружаем другую библиотеку
            E = dlsym(handle, "E");
            SinIntegral = dlsym(handle, "SinIntegral");
        } else if (t == 1)
        {
            float a, b, e;
            scanf("%f %f %f", &a, &b, &e);
            printf("SinIntegral: %f\n", (*SinIntegral)(a, b, e));
        } else if (t == 2)
        {
            int x;
            scanf("%d", &x);
            printf("E: %f\n", (*E)(x));
        }
    }
    return 0;
}