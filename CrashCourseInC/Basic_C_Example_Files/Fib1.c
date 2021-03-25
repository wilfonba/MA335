#include <stdlib.h>
#include <stdio.h>

 int main(int argc, char** argv){
  int i;
  int j=5;
  int f1=0;
  int f2=1;
  int f3=0;
  for (i=0;i<10;i++){
    f3=f2+f1;
    printf("The %d Fibonacci number is %d, j=%d\n",i,f3,j);
    f1=f2;
    f2=f3;
    j+=54;
  }

}
