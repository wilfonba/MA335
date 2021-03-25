#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

  int len;
  if (argc<2){
    printf("Give a length.\n");
    return 1;
  }
  //grab the second command line argument:
  sscanf(argv[1],"%d",&len);

  //make an array that is len long, then get rid of it.
  int* junk=malloc(len*sizeof(int));
  //fill it up with random junk

  int i;
  for (i=0;i<len;i++){
    junk[i]=i;
  }

  //free up junk
  free(junk);

  
  //make an array that is len long.
  int* my_array=malloc(len*sizeof(int));

  //add up the entries in my_array.
  int total=0;
  for (i=0;i<len;i++){
    if (i<10){
      printf("my_array[%d]=%d\n",i,my_array[i]);
    }
    total=total+my_array[i];
  }
  printf("The total was %d\n", total);
  return 0;

}
