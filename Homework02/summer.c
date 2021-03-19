#include <stdio.h>
#include <stdlib.h>

double getInput(int iii)
{
    double numb;
    char line[1000];
    printf("Enter number %d: \n",iii+1);
    fgets(line,1000,stdin);
    sscanf(line,"%lf",&numb);
    return numb;
}

int main()
{
    double sum = 0;
    for(int iii = 0;iii<3;iii++)
    {
        sum = sum + getInput(iii);
    }
    printf("%1.6lf\n",sum);
}