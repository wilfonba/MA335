#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void main(int argc, char **argv)
{
    double numbs[10] = {};
    char line[1000];
    printf("Please enter 10 numbers \n");
    for (int i=0;i<10;i++)
    {
        printf("Enter a number: ");
        fgets(line,1000,stdin);
        sscanf(line,"%lf",&numbs[i]);
    }
    double total = 0;
    for (int i=0;i<10;i++)
    {
        total += numbs[i];
    }
    printf("The average of those 10 numbers is: %6.4lf \n",total/10);
}