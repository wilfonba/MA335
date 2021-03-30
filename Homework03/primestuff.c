#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <strings.h>

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
    *num_mersenne = 0;
    *num_prime = 0;
    for(int iii = 0;iii < l;iii++)
    {
        if (isprime(*(ns+iii)) == 1)
        {
            *(primes+*num_prime) = *(ns + iii);
            *num_prime += 1;
            if (isMersenne(*(ns+iii)) == 1)
            {
                *(mersennes+*num_mersenne) = *(ns + iii);
                *num_mersenne += 1;
            }
        }
    }
}


int main(int argv, char **argc)
{
    
    return 0;
}