#include <stdlib.h>
#include <stdio.h>

void print_array(float* arr){
  int i;
  for (i=0;i<10;i++){
    printf("%f ",arr[i]);
  }
  printf("\n");
}



 int main(int argc, char** argv){
  float numberlist[10];
  int idx=0;
  float val;
  print_array(numberlist);
  printf("Which index would you like to change 
  (-1 to quit)? \n");
  scanf("%d",&idx);

  while (idx!=-1){
        printf("What value would you like to put in 
        index %d? ",idx);
        scanf("%f",&val);
        numberlist[idx]=val;
	print_array(numberlist);
        printf("Which index would you like to change 
        (-1 to quit)? \n");
        scanf("%d",&idx);
  }

 }
