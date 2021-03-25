#include<stdio.h>

int main(int argc,char** argv){

  printf("You supplied %d options.\n",argc);
  int i;
  for (i=0;i<argc;i++){
    printf("argument %d: %s\n",i,argv[i]);
  }
}
