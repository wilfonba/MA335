#include<stdio.h>
#include<stdlib.h>


void main(int argc,char** argv)
{

  int i;
  printf("The command line arguments are:\n");
  for (i=0;i<argc;i++){
    printf("%s\n",argv[i]);
  }

  
}

