#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char** argv){
  MPI_Init(&argc,&argv);
  int total_procs;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD,&total_procs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  int i;

  //Every processor has some data
  int mydata[3]={rank*2-5,rank*4+2,rank*5-6};
  printf("Rank %d Data: %d %d %d\n", rank,mydata[0],mydata[1],mydata[2]);

  //We want the sum of all the data on all the machines to end up on a certain node. 

  //Call MPI_Reduce
  if(rank==0) {
    int sum[3];
    MPI_Reduce(mydata,sum,3,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    printf("The sum of the first entries  is %d\n",sum[0]);
    printf("The sum of the second entries is %d\n",sum[1]);
    printf("The sum of the third entries is %d\n",sum[2]);
  } else {
    MPI_Reduce(mydata,NULL,3,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
  }


  MPI_Finalize();

}
