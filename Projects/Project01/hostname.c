#include<stdio.h>
#include<unistd.h>

int main(){
  char name[1000];
  gethostname(name,1000);
  printf("This is machine %s",name);
  return(0);
}
