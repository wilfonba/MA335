#include <stdio.h>
#include <stdlib.h>

int getInput()
{
    char line[1000];
    int valid = 0;
    int numb = 0;
    while(valid == 0)
    {
        printf("Enter a positive integer number of rows: ");
        fgets(line,1000,stdin);
        sscanf(line,"%d\n",&numb);
        if(numb > 0)
        {
            valid = 1;
        }
    }
    return numb;
}


int main()
{
    int N = getInput();
    for(int i = 0;i <= N;i++)
    {
        for(int j = 0;j < i;j++)
        {
            printf(" ");
        }
        for(int k = N - i;k > 0;k--)
        {
            printf("*");
        }
        printf("\n");
    }
    return 0;
}