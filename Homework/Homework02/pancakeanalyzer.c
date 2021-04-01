#include <stdio.h>
#include <stdlib.h>

struct max
{
    int cakes;
    int index;
};

struct min
{
    int cakes;
    int index;
};

int getInput(int iii)
{
    char line[1000];
    int cakes;
    printf("How many pancakes did person %d eat?\n\n",iii+1);
    fgets(line,1000,stdin);
    sscanf(line,"%d",&cakes);
    return cakes;
}

int main()
{
    printf("Welcome to pancake analyzer.  Your number one tool for analyzing pancake eating.\n");
    int cakes = 0;
    struct max maxCakes;
    maxCakes.cakes = -987654321;
    struct min minCakes;
    minCakes.cakes = 987654321;
    for(int iii = 0;iii < 10;iii++)
    {
        cakes = getInput(iii);
        if(cakes > maxCakes.cakes)
        {
            maxCakes.cakes = cakes;
            maxCakes.index = iii;
        }
        if(cakes < minCakes.cakes)
        {
            minCakes.cakes = cakes;
            minCakes.index = iii;
        }
    }
    printf("Person %d ate the fewest pancakes, with %d \n",minCakes.index + 1,minCakes.cakes);
    printf("Person %d ate the most pancakes, with %d \n",maxCakes.index + 1,maxCakes.cakes);
}