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

  //What is the "root" of the gather?  The root is the process that ends
  //up with all the data. 
  int root=0;

  //How long will a messge be?
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
    printf("Demonstrating MPI_Gather.\n");
    printf("Root of Gather: %d\n",root);
    printf("Message Length: %d\n\n",msg_len);
    fflush(stdout);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  //Everyone make up an arbitrary "message" of doubles. 
  int i;
  double* M=malloc(msg_len*sizeof(double));
  //Seed random number generator. 
  srand(time(NULL)+rank*10000);
  //Fill in your message.
  for (i=0;i<msg_len;i++) {
    M[i]=1.0*rand()/RAND_MAX;
  }

  //print out what message you have. 
  printf("Rank: %d Message: %s\n",rank,dbl_arr2str(M,msg_len));

  //If you are the root of the gather, you are going to recieve a full copy of all messages.
  //Make a buffer to accomodate them all. 
  if (rank==root) {
    double* recvbuf = malloc(total_procs*msg_len*sizeof(double));
    //Engage int the gather, specifying your recvbuf.
    //This says, "engage in a gather.  Contribute the data in M to the gather"
    //"M is msg_len long, and of type MPI_DOUBLE"
    //"Recieve data into recvbuf, I expect to recieve mesg_len data per processor, of
    // type MPI_DOUBLE, and the root of this gather is root on the world communicator" 
    MPI_Gather(M,msg_len,MPI_DOUBLE,recvbuf,msg_len,MPI_DOUBLE,root,MPI_COMM_WORLD);
    //print out the contents of your recvbuf to prove you have all the data. 
    printf("Rank %d ends with recvbuf: %s\n",rank,dbl_arr2str(recvbuf,msg_len*total_procs));
  } else {
    //If you aren't the root, you aren't getting any data at the end, so no need to make a recvbuf.
    //This says "Contribute M, which is of length msg_len and type MPI_DOUBLE, to the gather"
    //My recieve buffer is NULL, and I expect to recieve 0 MPI_DOUBLES per processor.  The root
    //of the gather is root, on the MPI_COMM_WORLD communicator. 
    MPI_Gather(M,msg_len,MPI_DOUBLE,NULL,0,MPI_DOUBLE,root,MPI_COMM_WORLD);

  }

  MPI_Finalize();

}
