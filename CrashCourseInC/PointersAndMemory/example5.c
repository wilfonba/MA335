#include<stdio.h>

int main(){
    int i;
    i=100;
    int* i_pointer=&i; //(i_pointer will have some crazy 
                       // value,say 11234568)
    *i_pointer=5901;   // change the value of the data 
                       // stored at 11234568 to be 5901.
          
    //since i is stored at location 11234568
    //the value of i will be changed.                    
    printf("%d\n",i);
}
