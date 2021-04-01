#include <stdio.h>
#include <stdlib.h>

int getInput(int iii)
{
    char line[1000];
    int cakes;
    printf("How many pancakes did person %d eat?\n\n",iii+1);
    fgets(line,1000,stdin);
    sscanf(line,"%d",&cakes);
    return cakes;
}

void bubblesort(int *cakes,int *idx,int N)
{
    int swapping = 1;
    int temp = 0;
    while(swapping == 1)
    {
        swapping= 0;
        for(int i = 0;i < N - 1;i++)
        {
            if(*(cakes + i) > *(cakes + 1 + i))
            {
                swapping = 1;
                temp = *(cakes + i);
                *(cakes + i) = *(cakes + i + 1);
                *(cakes + i + 1) = temp;
                temp = *(idx + i);
                *(idx + i) = *(idx + i + 1);
                *(idx + i + 1) = temp;
            }
        }
    }
}

int main()
{
    printf("Welcome to pancake analyzer PRO.  The essential tool for analyzing pancake eating.\n");
    int cakes[10];
    for(int iii = 0;iii < 10;iii++)
    {
        cakes[iii] = getInput(iii);
    }

    int idx[10];
    for(int j = 0;j < 10;j++)
    {
        idx[j] = j+1;
    }

    bubblesort(cakes,idx,10);
    for(int i = 0;i < 10;i++)
    {
        printf("Person %d ate %d pancakes\n",idx[i],cakes[i]);
    }
}