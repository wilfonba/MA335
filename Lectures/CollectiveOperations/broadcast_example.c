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


  //What is the "root" of the broadcast?  The root is the process that sends out the message.
  int root=0;

  //How long will the messge be?
  int msg_len=5;

  //specify the root of the gather, default to 0
  if (argc>1) {
    sscanf(argv[1],"%d",&root);
  }
  //Read message length if supplied.
  if (argc>2) {
    sscanf(argv[2],"%d",&msg_len);
  }

  //Print out the options.
  if (rank==0) {
    printf("Demonstrating MPI_Bcast.\n");
    printf("Root of broadcast: %d\n",root);
    printf("Message Length: %d\n\n",msg_len);
    fflush(stdout);
  }

  MPI_Barrier(MPI_COMM_WORLD);


  //If you are the root, make up an arbitrary message. Then, send it out to everyone via 
  //MPI_Bcast

  if (rank==root) {
    int i;
    int* M=malloc(msg_len*sizeof(int));
    srand(time(NULL)+rank*1000);
    for (i=0;i<msg_len;i++) {
      M[i]=rand() % 10;
    }
    printf("Rank %d has message M:%s\n",rank,int_arr2str(M,msg_len));
    //This says "Engage in a broadcast.  My send/recv buffer is M, which is msg_len long of type MPI_INT."
    //The root of the broadcast is root, since I am the root, treat M as a send buffer."
    MPI_Bcast(M,msg_len,MPI_INT,root,MPI_COMM_WORLD);
  } else {
    //If you aren't the root, make a spot to recive the message. 
    int* M=malloc(msg_len*sizeof(int));
    //Here, because your rank is not root, M is treated as a recieve buffer. 
    MPI_Bcast(M,msg_len,MPI_INT,root,MPI_COMM_WORLD);

    printf("Rank %d ends up with message M: %s\n",rank,int_arr2str(M,msg_len));
  }


  MPI_Finalize();

}
