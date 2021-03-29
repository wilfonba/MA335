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
  if (rank==0) {
    for(i=0;i<3;i++) {
      total=total+data[i];
    }

    int rbuf[2];
    MPI_Status status;
    printf("Rank 0 recieving a message from rank 1\n");
    MPI_Recv(rbuf,1,MPI_INT,1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
    printf("Rank 0 recieved data: %d from rank 1\n",rbuf[0]);
    total=total+rbuf[0];
  } else {
    for(i=3;i<6;i++) {
      total=total+data[i];
    }

    printf("Rank 1 sending data: %d to rank 0\n",total);
    MPI_Send(&total,1,MPI_INT,0,0,MPI_COMM_WORLD);
  }
  
  printf("I am process: %d Total: %d\n",rank, total);
 
  MPI_Finalize();
  return(0);
}
