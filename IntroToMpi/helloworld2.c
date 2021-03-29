#include<mpi.h>
#include<stdio.h>

int main(int argc, char** argv){
  MPI_Init(&argc,&argv);
  int total_procs;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD,&total_procs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  if (rank==0) {
    printf("Hello, this is process %d  and I'm the master.\n",rank);
  } else {
    printf("Hello, this is process %d  and I'm a lowly worker. :(\n",rank);
  }
  MPI_Finalize();
}
