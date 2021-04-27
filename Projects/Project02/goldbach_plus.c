#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include "goldbach_plus_helpers.c"

  /*TAG  -  USE
  0   -  Sending a task to a worker node.
  1   -  Sending the first part (ID and length) from a worker to the root.
  2   -  Sending the last part (list of primes) for a result from a worker to
          the root. 
  3   -  Root sending a quit message to a worker.*/
enum messageTags {
  SEND_TASK = 0,
  REQUEST_TASK = 1,
  TAG_QUIT = 2,
  KILL_TAG = 3,
};

long int  goldbach_partition_calls=0;

//returns 1 if n is prime, 0 otherwise
int isprime(long int n){
  if (n<2){
    return 0;
  }
  long int i=2;
  while (i*i<=n){
    if (n%i==0){
      return 0;
    }
    i++;
  }
  return 1;
}

//Attempts to calculate a Golbach partition of n.
//Returns a Goldbach partition that includes the smallest possible prime.
//i.e., both 3/7 and 5/5 are partitions of 10.  3/7 is returned because 3 is the smallest of 3,5,7.
//Returns 1 on success, in this case a and b hold the partition
//Returns 2 if n<=2, or if n is odd
//Returns 3 if you have disproved Golbach's conjecture. 
long int goldbach_partition(long int n, long int* a, long int* b){
  goldbach_partition_calls++;
  int exit_code=2;
  if (n<=2 || (n % 2 ==1)){
    return exit_code;
  }

  exit_code=3;

  long int i;
  for (i=2;i<=n/2;i++){
    if (isprime(i) && isprime(n-i)){
      *a=i; *b=n-i;
      exit_code=1;
      return exit_code;
    }
  }

  return exit_code;
 
}

long int min(long int a, long int b){
  return a<b ? a:b;
}

void sendQuit(int rank) {
  MPI_Send(NULL,0,MPI_INT,rank,TAG_QUIT,MPI_COMM_WORLD);
}

void sendTask(long int* taskID, options* opt,int dest) {
  MPI_Send(taskID,3,MPI_LONG,dest,SEND_TASK,MPI_COMM_WORLD);
}

int my_rank() {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  return rank;
}

void rootStuff(options* opt) {
  int rank = my_rank();
  int numProcs;
  MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
  // Keeping track of tasks assigned and completed as well as describing them
  // with as (id, lower index, upper index)
  long int lastTask[3] = {-1,1,-1};
  // Assign each worker an initial task unless there are none left to assign
  long int i = 0;
  for(i = 1;i < numProcs;i++) {
    lastTask[0]++;
    lastTask[1] = lastTask[2] + 1;
    lastTask[2] = lastTask[1]+opt->chunksize - 1;
    //printf("Sending task [%d,%d,%d]\n",lastTask[0],lastTask[1],lastTask[2]);
    sendTask(lastTask,opt,i);
  }
  // Allocate remaining tasks
  long int SUCCESS[3] = {0,0,0};
  while(1) {
    MPI_Status status;
    // Receive request for new task
    MPI_Recv(&SUCCESS,3,MPI_LONG,MPI_ANY_SOURCE,REQUEST_TASK,MPI_COMM_WORLD,&status);
    int workerID = status.MPI_SOURCE;
    // Assign requesting core a new task if N(p) hasn't been found
    if (SUCCESS[0] == 0) {
      lastTask[0]++;
      lastTask[1] = lastTask[2] + 1;
      lastTask[2] = lastTask[1]+opt->chunksize-1;
      sendTask(lastTask,opt,workerID);
    }
    // Send quit message is N(p) has been found
    else {
      sendQuit(workerID);
      int idx = 0;
      long int N = SUCCESS[0];
      long int a = SUCCESS[1];
      long int b = SUCCESS[2];
      for (i = 1;i<numProcs;i++) {
        if (i != workerID) {
          MPI_Recv(&SUCCESS,3,MPI_LONG,i,REQUEST_TASK,MPI_COMM_WORLD,&status);
          if (SUCCESS[0] < N && SUCCESS[0] != 0) {
            N = SUCCESS[0];
            a = SUCCESS[1];
            b = SUCCESS[2];
          }
          sendQuit(i);
        }
      }
      printf("N(%d)=%d: %d/%d\n",opt->p,N,a,b);
      break;
    }
  }
}

void workerStuff(options* opt) {
  int rank = my_rank();
  MPI_Status status;
  long int recvBuff[3] = {0,0,0};
  while (1) {
    MPI_Recv(recvBuff,3,MPI_LONG,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
    if (status.MPI_TAG == TAG_QUIT) {
      break;
    }
    long int i;
    long int a;
    long int b;
    int isGoldbach;
    long int SUCCESS[3] = {0,0,0};
    //printf("Rank %d receiving task [%d,%d,%d]\n",rank,recvBuff[0],recvBuff[1],recvBuff[2]);
    for (i = recvBuff[1];i <= recvBuff[2];i++) {
      isGoldbach = goldbach_partition(i,&a,&b);
      switch (isGoldbach) {
        case 1:
          if (a >= opt->p)
          {
            //printf("N(%d)=%d: %d/%d\n",opt->p,i,a,b);
            SUCCESS[0] = i;
            SUCCESS[1] = a;
            SUCCESS[2] = b;
            break;
          }
        case 2:
          break;
        case 3:
          printf("You disproved Goldbach's Conjecture! YAY!!!\n");
          break;
        default:
          printf("Something went wrong with getting Goldbach partitions...\n");
      }
      if (SUCCESS[0] != 0) {
        break;
      }
    }
    MPI_Send(&SUCCESS,3,MPI_LONG,0,REQUEST_TASK,MPI_COMM_WORLD);
  }
}


int main(int argc, char** argv){

  MPI_Init(&argc,&argv);

  int rank = my_rank();
  int size;
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  
  options opt;
  parse_options(argc,argv,&opt);
  if (rank==0){
    print_options(&opt);
    rootStuff(&opt);
  }
  else {
    workerStuff(&opt);
  }
  //Now the options are properly read and stored in opt.  The field you care about it
  //opt.p, which holds the value of p for which we want to calculate N(p)
  //opt.chunksize is also populated, you may pay attention to it if you wish. 
  MPI_Finalize();
}
