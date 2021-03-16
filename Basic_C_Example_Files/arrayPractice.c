#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    char line[1000] = {};
    int N = 0;
    printf("Please enter an integer N to define the array size: ");
    fgets(line,1000,stdin);
    sscanf(line,"%d",&N);
    double A[N];
    double B[N];
    double C[N];

    for(int i = 0;i <= N;i++)
    {
        A[i] = sin(i);
        B[i] = cos(i);
        C[i] = pow(A[i],2) + pow(B[i],2);
    }

    return 0;
}