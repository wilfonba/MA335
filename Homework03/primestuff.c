#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int isprime(int N)
{
    if (N < 2)
    {
        return 0;
    }
    if (N <= 3)
    {
        return 1;
    }
    else
    {
        for(int i = 2;i <= (N/2)+1;i++)
        {
            if(N%i == 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

int isMersenne(int N)
{
    if (isprime(N) == 1)
    {
        if (floor(log2(N+1)) == log2(N+1))
        {
            return 1;
        }
    }
    return 0;
}

void prime_info(int* ns, int l, int* num_prime, int* primes, int* num_mersenne, int* mersennes)
{
    int a = 1;
    a+=1;
}


int main(int argv, char **argc)
{
    isprime(3);
    isMersenne(3);
    return 0;
}