#include<stdio.h>
#include<stdlib.h>

int main(int argc,char** argv){

  printf("One plus the input is %d\n",
	 atoi(argv[1])+1);

  int value;
  sscanf(argv[1],"%d",&value);
  printf("one plus the input is %d\n",value+1);

  return 0;
  

}
