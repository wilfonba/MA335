#include<stdio.h>

int main(){

    int nums[3];
    *(nums+0)=11; 
    *(nums+1)=6;
    *(nums+2)=0;
    printf("%d %d %d\n",nums[0],nums[1],nums[2]);

}
