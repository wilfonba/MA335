#include<stdio.h>

int main(){
    int i;
    i=100;
    int* i_pointer=&i; //(i_pointer will have some crazy 
                       // value,say 11234568)
    int j;
    j=*i_pointer; //(j will have the value of the integer 
                  // stored at position 11234568, which 
                  // happens to be 100.)
    printf("The value of j is %d\n",j);
}
