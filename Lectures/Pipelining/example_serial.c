
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void rand_int_arr(int* arr, int n){
  int i;
  for (i=0;i<n;i++){
    arr[i]=rand()%100;
  }

}


int do_adding(int a,int p){
  int result=a;
  int i;
  for (i=1;i<=p;i++){
    result+=i;
  }
  return result;
}
  
void main(int argc,char**argv){

  //int seed=time(NULL);
  int seed=6;
  srand(seed);
  int n=1000000;
  int m=4;
  int *init_vals=malloc(n*sizeof(int));
  rand_int_arr(init_vals,n);

  int results[n];
  int i;
  for (i=0;i<n;i++){
    if (i==0){
      results[i]=do_adding(init_vals[i], m);
    } else {
      results[i]=do_adding(init_vals[i]+results[i-1],m);
    }
    //printf("%d\n",results[i]);
  }
  printf("last result: %d\n", results[i-1]);

  exit(0);
}
