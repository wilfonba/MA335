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

  //What is the "root" of the reduce?  The root is the process that ends up with the result.
  int root=0;

  //How long will the messages be?
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
    printf("Demonstrating MPI_Reduce.\n");
    printf("Root of reduce: %d\n",root);
    printf("Message Length: %d\n",msg_len);
    printf("Operation: Max\n\n");
    fflush(stdout);
  }

  MPI_Barrier(MPI_COMM_WORLD);



  //Everyone make a message. 
  int i;
  int* M=malloc(msg_len*sizeof(int));
  srand(time(NULL)+rank*1000);
  for (i=0;i<msg_len;i++) {
    M[i]=rand() % 20;
  }
  printf("Rank %d has message M:%s\n",rank,int_arr2str(M,msg_len));
  fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);

  //make a place to store the results.  If you are not the root, you can leave this null. 
  //if you are the root, better allocate it. 
  int* A=NULL;
  if (rank==root) {
    A=malloc(msg_len*sizeof(int));
  }

  //Call MPI_Reduce.  M is the data I am contributing, and A is where the answer will be placed.
  //MPI_MAX means that the operation we are applying is maximum. 
  MPI_Reduce(M,A,msg_len,MPI_INT,MPI_MAX,root,MPI_COMM_WORLD);
  
  //If you are the root, print out the result:
  if (rank==root) {
    printf("Rank %d recieves: %s\n",rank,int_arr2str(A,msg_len));
  }


  MPI_Finalize();

}
