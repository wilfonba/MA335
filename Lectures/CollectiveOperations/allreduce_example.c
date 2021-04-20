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

  //Read message length if supplied.
  if(argc>1) {
    sscanf(argv[1],"%d",&msg_len);
  }

  //Print out the options.
  if(rank==0) {
    printf("Demonstrating MPI_Allreduce.\n");
    printf("Root of reduce: %d\n",root);
    printf("Message Length: %d\n",msg_len);
    printf("Operation: +\n\n");
    fflush(stdout);
  }

  MPI_Barrier(MPI_COMM_WORLD);



  //Everyone make a message. 
  int i;
  int* M=malloc(msg_len*sizeof(int));
  srand(time(NULL)+rank*1000);
  for(i=0;i<msg_len;i++) {
    M[i]=rand() % 20;
  }
  printf("Rank %d has message M:%s\n",rank,int_arr2str(M,msg_len));
  fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);

  //*Everyone* make a place to store the results. 
  int* A=malloc(msg_len*sizeof(int));

  //Call MPI_Allreduce.  M is the data I am contributing, and A is where the answer will be placed.
  //MPI_SUM means that the operation we are applying is +. 
  MPI_Allreduce(M,A,msg_len,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
  
  //print out the result:
  printf("Rank %d recieves: %s\n",rank,int_arr2str(A,msg_len));
  
  MPI_Finalize();

}
