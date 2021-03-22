#include <stdio.h>
#include <stdlib.h>

void swap(int *p_x, int *p_y)
{
    int temp = *p_x;
    *p_x = *p_y;
    *p_y = temp;
}

int stringLen(char *s)
{
    int length = 0;
    int term = 0;
    while (term != 1)
    {
        if (*(s+length) == '\0')
        {
            return length - 1;
        }
        length += 1;
    }
}


int main(int argc,char **argv)
{
    int a = 1;
    int b = 2;
    printf("A = %d - B = %d \n",a,b);
    printf("Swapping...\n");
    swap(&a,&b);
    printf("A = %d - B = %d \n",a,b);

    char str[20] = "This is a string.";
    printf("The length of the string 'This is a string' is %d.\n",stringLen(str));

    return 0;
}