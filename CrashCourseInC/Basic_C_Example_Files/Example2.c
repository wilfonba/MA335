#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv){
  double x;
  double y;
  printf("Enter x: \n");
  scanf("%lf",&x);
  y=g(x);
  printf("g(x)=%lf\n",y);
  return 0;
}

double g(double x){
  return pow(x,2)+3*x-1;
}



