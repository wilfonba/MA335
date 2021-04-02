#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "helpers_static.h"


int is_prime(int n) {
  int i;
  if(n<=1) {
    return 0;
  }
  for(i=2;i*i<n;i++) {
    if (n%i==0){
      return 0;
    }
  }

  return i;
}


void send_results(int* results, int min, int max) {

 
}

void recv_results(int* results) {
  
}

void workerstuff(options* o) {

}


void masterstuff(options* o) {

}


int main(int argc, char** argv) {
  MPI_Init(&argc,&argv);
  int rank;
  int numproc;

  MPI_Comm_size(MPI_COMM_WORLD,&numproc);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  
  options o;
  set_opts(argc,argv,&o);
  if (rank==0){
    print_opts(&o);
    masterstuff(&o);
  } else {
    workerstuff(&o);
  }
  
  MPI_Finalize();
  return 0;
}
