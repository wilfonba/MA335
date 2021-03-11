#include <stdlib.h>
#include <stdio.h>

 int main(int argc, char** argv){
  double max;
  int i=0; int f1=0; int f2=1; int f3=0; 
  printf("Enter value past which you do not care about Fibonacci numbers: \n");
  scanf("%lf",&max);
  printf("max is %lf\n",max);
  i=2;
  while (f3<max){
    printf("The %d Fibonacci number is %d\n",i,f3);
    f1=f2;
    f2=f3;
    f3=f1+f2;
    i=i+1;
  }
}
