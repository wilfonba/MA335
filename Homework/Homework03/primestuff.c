#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

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

void printArray(int *ar, int l)
{
    printf("[");
    if (l != 0)
    {
        for(int iii = 0;iii < l - 1;iii++)
        {
            printf("%d,",*(ar+iii));
        }
        printf("%d]\n",*(ar+l-1));
        return;
    }
    printf("]\n");
}

void printPrimeInfo(int* ns, int l, int* num_prime, int* primes, int* num_mersenne, int* mersennes)
{
    printf("Consider the array ");
    printArray(ns,l);
    printf("There are %d prime elements, they are ",*num_prime);
    printArray(primes,*num_prime);
    printf("There are %d Mersenne prime elements, they are ",*num_mersenne);
    printArray(mersennes,*num_mersenne);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("See usage information\n");
        return 0;
    }
    if (strcmp(argv[1],"--list") == 0)
    {
        int N = argc - 2;
        int *primes=(int*)malloc(N*sizeof(int));
        int *mersennes=(int*)malloc(N*sizeof(int));
        int num_prime = 0;
        int num_mersenne = 0;
        int *numbers=(int*)malloc(N*sizeof(int));
        for(int iii=0;iii < N;iii++)
        {
            numbers[iii] = atoi(argv[iii+2]);
        }
        prime_info(numbers,N,&num_prime,primes,&num_mersenne,mersennes);
        printPrimeInfo(numbers,N,&num_prime,primes,&num_mersenne,mersennes);
    }
    else if (strcmp(argv[1],"--range") == 0)
    {
        if (argc < 4)
        {
            printf("See usage information\n");
            return 0;
        }
        int min = atoi(argv[2]);
        int max = atoi(argv[3]);
        int N = max - min + 1;
        int *numbers=(int*)malloc(N*sizeof(int));
        int *primes=(int*)malloc(N*sizeof(int));
        int *mersennes=(int*)malloc(N*sizeof(int));
        int num_prime = 0;
        int num_mersenne = 0;
        int itr = 0;
        for(int iii = min;iii <= max;iii++)
        {
            numbers[itr] = iii;
            itr++;
        }
        prime_info(numbers,N,&num_prime,primes,&num_mersenne,mersennes);
        printPrimeInfo(numbers,N,&num_prime,primes,&num_mersenne,mersennes);
    }
    return 0;
}