#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>


int main(int argc, char** argv){

  int numproc;
  int rank;
  
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numproc);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  int data[6]={12,10,3,2,9,0};
  int i;
  int total=0;
  if (rank==0){
    for (i=0;i<3;i++) {
      total=total+data[i];
    }
  } else{
    for (i=3;i<6;i++) {
      total=total+data[i];
    }
  }
  
  printf("I am process: %d Total: %d\n",rank, total);
  
  MPI_Finalize();
  return(0);
  
}
