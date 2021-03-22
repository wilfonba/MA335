#include<stdlib.h>
#include<stdio.h>

void myfunction(long long int chunksize){
  
  int* arr=calloc(chunksize,sizeof(int));
  if (arr==NULL){
    printf("It Quit.\n");
    exit(0);
  }
  long long int i=0;
  for (i=0;i<chunksize;i++){
    arr[i]=i%10;
  }

  free(arr);

}


void main(int argc, char** argv){

  if (argc < 3){
    printf("Usage: memory_leak <chunk_size> <times to allocate>\n");
    exit(1);
  }

  long long int chunksize;
  sscanf(argv[1],"%lld",&chunksize);

  int timestomalloc=atoi(argv[2]);
 
  printf("Asking for %s MB/per allocation\n",argv[1]);
  printf("Doing %d allocations\n",timestomalloc);

  //You can also use atoi.  I just like sscanf better I guess. 
  int i;
  for (i=0;i<timestomalloc;i++){
    myfunction(chunksize);
    printf("Succeeded in allocating and using the memory I needed\n");
  }

}
