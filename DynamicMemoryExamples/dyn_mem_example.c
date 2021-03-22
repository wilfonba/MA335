#include <stdio.h>
#include <stdlib.h>

// Shamelessly stolen from http://www.programiz.com/c-programming/c-dynamic-memory-allocation
int main(){
    int n=0;
    int i=0;
    int sum=0;
    printf("Enter number of elements: ");
    scanf("%d",&n);
    int* ptr;
    ptr=(int*)malloc(n*sizeof(int));  //memory allocated using malloc
    if(ptr==NULL)                     
    {
        printf("Error! memory not allocated.");
        exit(0);
    }
    //int ptr[n];
    printf("Enter elements of array: ");
    for(i=0;i<n;++i)
    {
        scanf("%d",&(ptr[i]));
        sum+=ptr[i];
    }
    printf("Sum=%d\n",sum);
    free(ptr);
    return 0;
}
