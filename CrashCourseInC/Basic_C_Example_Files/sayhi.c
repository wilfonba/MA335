#include <stdio.h>
#include <unistd.h>

void main(){
  int i;
  char name[1000];
  char cmpname[1000];
  int j;

  getlogin_r(name, 1000);
  gethostname(cmpname,1000);
  printf("Hello %s. You are using %s\n",name,cmpname);
  
  

}
