#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "arr2str.c"

int main(int argc, char** argv){
  MPI_Init(&argc,&argv);
  int total_procs;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD,&total_procs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);


  //How long will the messges be?
  int msg_len=5;

  //Read message length if supplied.
  if (argc>1) {
    sscanf(argv[1],"%d",&msg_len);
  }

  //Print out the options.
  if (rank==0) {
    printf("Demonstrating MPI_Alltoall.\n");
    printf("Message Length: %d\n\n",msg_len);
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  //Make up a message.
  int i;
  srand(time(NULL)+rank*10000);
  int* M=malloc(msg_len*sizeof(int));
  for (i=0;i<msg_len;i++) {
    M[i]=rand() % 10;
  }

  //Make a place to store all the messages. 
  int* recvbuf=malloc(msg_len*total_procs*sizeof(int));

  //Print the message you are contributing. 
  printf("Rank %d contributes message M:%s\n",rank,int_arr2str(M,msg_len));
  fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);
  //Engage in an all-to-all broadcast.  Send the data in M, and put the collection of all 
  //messages in recvbuf. 
  MPI_Allgather(M,msg_len,MPI_INT,recvbuf,msg_len,MPI_INT,MPI_COMM_WORLD);

  //print out what you have. 
  printf("Rank %d ends up with colllection: %s\n",rank,int_arr2str(recvbuf,msg_len*total_procs));
  MPI_Finalize();

}
