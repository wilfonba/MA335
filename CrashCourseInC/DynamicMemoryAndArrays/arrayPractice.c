#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    int N = atoi(*(argv + 1));

    // Data entry
    double *numbs=(double*)malloc(N*sizeof(double));
    if (numbs == NULL)
    {
        printf("Memory allocation failed \n");
        return 1;
    }
    printf("Please enter %d numbers.\n",N);
    char line[100];
    for (int i=0;i<N;i++)
    {
        printf("Enter a number: ");
        fgets(line,100,stdin);
        sscanf(line,"%lf",(numbs + i));
    }

    // Calculate and output results
    double *A=(double*)malloc(N*sizeof(double));
    double *B=(double*)malloc(N*sizeof(double));
    double *C=(double*)malloc(N*sizeof(double));

    printf("  entry |   A[i]  |   B[i]  |   C[i]  \n");
    for(int i = 0;i < N;i++)
    {
        *(A + i) = sin(*(numbs + i));
        *(B + i) = cos(*(numbs + i));
        *(C + i) = pow(*(A + i),2) + pow(*(B + i),2);
        printf("%+3.4lf | %+1.4lf | %+1.4lf | %+1.4lf\n",*(numbs + i),*(A + i),*(B + i),*(C + i));
    }

    return 0;
}