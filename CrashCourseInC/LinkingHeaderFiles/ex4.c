#include <stdio.h>
#include <stdlib.h>
#include "g.c"

int main(){
  double x=1;
  double y;
  y=g(x);

  printf("g(%lf)=%lf\n",x,y);
  return 0;
}
