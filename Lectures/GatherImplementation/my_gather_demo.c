#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arr2str.c"
#include "my_gather.c"


void main(int argc,char** argv){
  MPI_Init(&argc,&argv);
  int total_procs;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD,&total_procs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  //What is the "root" of the gather?  The root is the process that ends
  //up with all the data. 
  int root=0;

  //How long will a messge be?
  int msg_len=5;

  //default to 0, otherwise take as a parameter
  if(argc>1) {
    sscanf(argv[1],"%d",&root);
  }
  //Read message length if supplied.
  if(argc>2) {
    sscanf(argv[2],"%d",&msg_len);
  }

  //Print out the options.
  if(rank==0) {
    printf("Demonstrating my_gather.\n");
    printf("Root of Gather: %d\n",root);
    printf("Message Length: %d\n\n",msg_len);
    fflush(stdout);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  //Everyone make up an arbitrary "message" of doubles. 
  int i;
  int* M=malloc(msg_len*sizeof(int));
  //Seed random number generator. 
  srand(time(NULL)+rank*10000);
  //Fill in your message.
  for(i=0;i<msg_len;i++) {
    M[i]=rand()%50;
  }

  //print out what message you have. 
  printf("Rank: %d Message: %s\n",rank,int_arr2str(M,msg_len));

  MPI_Barrier(MPI_COMM_WORLD);
  //If you are the root of the gather, you are going to recieve a full copy of
  //all messages.
  //Make a buffer to accomodate them all. 
  if(rank==root) {
    int* recvbuf = malloc(total_procs*msg_len*sizeof(int));
    //Engage in the gather, specifying your recvbuf.
    //This says "Send the data in M, which is msg_len INTS long (INTS is implied
    //in my_gather), and store the results in recvbuf.  The root of the gather
    //is root."
    my_gather(M,msg_len,recvbuf,root);
    //print out the contents of your recvbuf to prove you have all the data. 
    printf("Rank %d ends with recvbuf: %s\n",rank,int_arr2str(recvbuf,msg_len*total_procs));
  } else {
    //If you aren't the root, you aren't getting any data at the end, so no need
    //to make a recvbuf.
    //This says "Contribute M, which is of length msg_len and type MPI_INT
    //(implied by my_gather), recieve buffer is NULL, because I'm not the root. 
    my_gather(M,msg_len,NULL,root);
  }

  MPI_Finalize();

}
