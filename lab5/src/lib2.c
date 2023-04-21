#include <stdio.h>

float binPow(float x, int y)
{
    float z = 1.0;
    while (y > 0)
    {
        if (y % 2 != 0)
        {
            z *= x;
        }
        x *= x;
        y /= 2;
    }
    return z;
}

float Sin(float x)
{
    float result = 0.0;
    int n = 0;
    while (n <= 10)
    {
        float numerator = binPow(-1, n) * binPow(x, 2 * n + 1);
        float denominator = 1.0;
        for (int i = 1; i <= 2 * n + 1; i++)
        {
            denominator *= i;
        }
        result += numerator / denominator;
        n++;
    }
    return result;
}

float SinIntegral(float A, float B, float e)
{
    float sum = (Sin(A) + Sin(B)) / 2.0;
    float x = A + e;
    while (x < B)
    {
        sum += Sin(x);
        x += e;
    }
    return sum * e;
}

int fact(int x)
{
    int res = 1;
    for (int i = 2; i <= x; i++)
    {
        res *= i;
    }
    return res;
}

float E(int x)
{
    float sum = 0;
    for (int i = 0; i <= x; i++)
    {
        sum += (1 / (float)fact(i));
    }
    return sum;
}
