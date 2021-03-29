#include<stdio.h>
#include<stdlib.h>

int main(){

  int data[6]={12,10,3,2,9,0};
  int i;
  int total=0;
  for (i=0;i<6;i++) {
    total=total+data[i];
  }
  
  printf("Total: %d\n",total);

  return(0);
  
}
