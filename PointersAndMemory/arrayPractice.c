#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void calc_C(int N,double* numbs,double* A,double* B,double* C)
{
    for(int i=0;i<N;i++)
    {
        *A = sin(*numbs);
        *B = cos(*numbs);
        *C = pow(*A,2) + pow(*B,2);
        C++;
        B++;
        A++;
        numbs++;
    }
}



int main(int argc, char **argv)
{
    // Determine number of numbers to enter
    char line[1000] = {};
    int N = 0;
    printf("Please enter an integer N to define the array size: ");
    fgets(line,1000,stdin);
    sscanf(line,"%d",&N);

    // Data entry
    double numbs[N];
    printf("Please enter %2d numbers.\n",N);
    for (int i=0;i<N;i++)
    {
        printf("Enter a number: ");
        fgets(line,1000,stdin);
        sscanf(line,"%lf",&numbs[i]);
    }

    // Calculate and output results
    double A[N];
    double B[N];
    double C[N];

    printf("  entry |   A[i]  |   B[i]  |   C[i]  \n");
    calc_C(N,numbs,A,B,C);
    for(int i = 0;i < N;i++)
    {
        //A[i] = sin(numbs[i]);
        //B[i] = cos(numbs[i]);
        //C[i] = pow(A[i],2) + pow(B[i],2);
        printf("%+3.4lf | %+1.4lf | %+1.4lf | %+1.4lf\n",numbs[i],A[i],B[i],C[i]);
    }

    return 0;
}