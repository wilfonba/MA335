#include <stdio.h>
#include <stdlib.h>

int getInput()
{
    int N;
    char line[1000];
    printf("Enter n, n>=0:\n");
    fgets(line,1000,stdin);
    sscanf(line,"%d",&N);
    return N;
}



int main()
{
    int N;
    int valid = 0;
    while(valid == 0)
    {
        N = getInput();
        if(N >= 0)
        {
            valid = 1;
        }
    }
    int u = 3;
    for(int iii = 0;iii < N;iii++)
    {
        u = 3*u + 4;
    }
    printf("u(%d)=%d\n",N,u);
}