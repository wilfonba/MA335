#include<stdlib.h>
#include<stdio.h>

int countEven(int* arr, int size)
{
    int count = 0;
    for(int iii = 0;iii < size;iii++)
    {
        if (*(arr+iii)%2 == 0)
        {
            count += 1;
        }
    }
    return count;
}

double* maximum(double* a, int size)
{
    double max = -987654321;
    int idx = 0;
    for(int iii = 0;iii < size;iii++)
    {
        if (*(a+iii)>max)
        {
            idx = iii;
            max = *(a+iii);
        }
    }
    return (a+idx);
}

int myStrLen(char* s)
{
    int length = 0;
    int searching = 1;
    while(searching == 1)
    {
        if (*(s+length) == '\0')
        {
            return length;
        }
        length++;
    }
    return 1;
}

void revString(char* s)
{
    int N = myStrLen(s) - 1;
    char temp;
    if (N < 1) 
    {
        return;
    }
    for (int iii = 0;iii < (N+1)/2;iii++)
    {
        temp = *(s + iii);
        *(s + iii) = *(s + N - iii);
        *(s + N - iii) = temp;
    }
    return;
}

void delEven(int* arr,int size)
{
    for (int iii = 0;iii < size;iii++)
    {
        if (*(arr+iii)%2 == 0)
        {
            *(arr+iii) = -1;
        }
    }
}

int main()
{
    printf("---------------------------------\n");
    printf("---   Testing countEven   -------\n");
    printf("---------------------------------\n\n");
    int x[15] = {2,2,4,4,6,6,8,8,10,10,12,12,14,14,14};
    int y[10] = {1,3,5,7,9,11,9,7,5,3};
    int z[15] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    printf("\n\nTesting countEven case x:\n");
    printf("Correct Answer: 15\n");
    printf("Programs Answer: %d\n\n",countEven(x,15));
    printf("\n\nTesting countEven case y:\n");
    printf("Correct Answer: 0\n");
    printf("Programs Answer: %d\n\n",countEven(y,10));
    printf("\n\nTesting countEven case z:\n");
    printf("Correct Answer: 7\n");
    printf("Programs Answer: %d\n\n",countEven(z,15));

    printf("---------------------------------\n");
    printf("---   Testing maximum   ---------\n");
    printf("---------------------------------\n\n");
    double a[5] = {1.23,5,5.43,2.34,101.3};
    double b[4] = {3.1415,3.3,3.23,2.71};
    double c[5] = {-100002,-10003.2,-23.4,-584.5,-54.21};
    printf("\n\nTesting maximum case a:\n");
    printf("Correct Answer: 101.3\n");
    printf("Program's Answer: %lf\n\n",*maximum(a,5));
    printf("\n\nTesting maximum case b:\n");
    printf("Correct Answer: 3.3\n");
    printf("Program's Answer: %lf\n\n",*maximum(b,4));
    printf("\n\nTesting maximum case c:\n");
    printf("Correct Answer: -23.4\n");
    printf("Program's Answer: %lf\n\n",*maximum(c,5));

    printf("---------------------------------\n");
    printf("---   Testing myStrLen   --------\n");
    printf("---------------------------------\n\n");
    char d[100] = "This is a string";
    char e[100] = "This is also a string";
    char f[100] = "Yet another string... Yay!!";
    printf("\n\nTesting myStrLeng case d\n");
    printf("Correct Answer: 16\n");
    printf("Program's Answer: %d\n\n",myStrLen(d));
    printf("\n\nTesting myStrLeng case e\n");
    printf("Correct Answer: 21\n");
    printf("Program's Answer: %d\n\n",myStrLen(e));
    printf("\n\nTesting myStrLeng case f\n");
    printf("Correct Answer: 27\n");
    printf("Program's Answer: %d\n\n",myStrLen(f));

    return 0;
}