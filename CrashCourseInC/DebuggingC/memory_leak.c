#include<stdlib.h>
#include<stdio.h>

void summer(long long int chunksize){
  
  int* arr=calloc(chunksize,sizeof(int));
  if (arr==NULL){
    printf("It Quit.\n");
    exit(0);
  }
  long long int i=0;
  for (i=0;i<chunksize;i++){
    arr[i]=i%10;
  }
  
}


void main(int argc, char** argv){

  long long int chunksize=1024*1024*atoi(argv[1]);
  printf("Asking for %s MB/per allocation\n",argv[1]);
  int timestomalloc=atoi(argv[2]);
  int i;
  for (i=0;i<timestomalloc;i++){
    summer(chunksize);
    printf("Succeeded in allocating and using the memory I needed\n");
  }

}
