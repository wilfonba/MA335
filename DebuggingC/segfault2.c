#include<stdio.h>
#include<stdlib.h>



void main(int argc,char** argv)
{


  int* myarray=malloc(201*sizeof(int));
  int i;

  for (i=0;i<=200;i++){
    myarray[i]=1000;
  }

  printf("%d\n",myarray[200]);
  free(myarray);
}

