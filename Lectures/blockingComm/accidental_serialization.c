#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>


//Pass data along -- move data one rank to the right.
//message size is a command line option. 
int main(int argc,char** argv) {
  MPI_Init(&argc,&argv);
  int rank;
  int size;
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  
  int message_size_KB=1;
  if (argc > 1) {
      message_size_KB=atoi(argv[1]);
  }

  if (rank==0) {
    fprintf(stderr,"These transactions are of size %d KB\n", message_size_KB);
  }

  int message_size_B=message_size_KB*1024;
  char* message=malloc(message_size_B);


  int dest=(rank+1) % size;
  int source=(rank-1+size)%size; 


  if (rank==0) {
    fprintf(stderr,"Rank %d started recv from %d\n",rank,source);
    MPI_Recv(message,message_size_B,MPI_BYTE,source,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    fprintf(stderr,"Rank %d recv'd from rank %d\n",rank,source);

    fprintf(stderr,"Rank %d started send to %d\n",rank,dest);
    MPI_Send(message,message_size_B,MPI_BYTE,dest,0,MPI_COMM_WORLD);
    fprintf(stderr,"Rank %d sent message to rank %d\n",rank,dest);
  } else {
    fprintf(stderr,"Rank %d started send to %d\n",rank,dest);
    MPI_Send(message,message_size_B,MPI_BYTE,dest,0,MPI_COMM_WORLD);
    fprintf(stderr,"Rank %d sent message to rank %d\n",rank,dest);

    fprintf(stderr,"Rank %d started recv from %d\n",rank,source);
    MPI_Recv(message,message_size_B,MPI_BYTE,source,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    fprintf(stderr,"Rank %d recv'd from rank %d\n",rank,source);
  }
  
  fprintf(stderr,"Rank %d done.\n",rank);
  
  MPI_Finalize();

}
