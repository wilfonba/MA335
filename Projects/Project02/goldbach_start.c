#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include "goldbach_helpers.c"


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
};

int goldbach_partition_calls=0;

//returns 1 if n is prime, 0 otherwise
int isprime(int n){
  if (n<2){
    return 0;
  }
  int i=2;
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
int goldbach_partition(int n, int* a, int* b){
  goldbach_partition_calls++;
  int exit_code=2;
  if (n<=2 || (n % 2 ==1)){
    return exit_code;
  }

  exit_code=3;

  int i;
  for (i=2;i<=n/2;i++){
    if (isprime(i) && isprime(n-i)){
      *a=i; *b=n-i;
      exit_code=1;
      return exit_code;
    }
  }

  return exit_code;
 
}

int min(int a, int b){
  return a<b ? a:b;
}

int my_rank() {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  return rank;
}

void sendTask(int* lastTask,int nData, options* opt,int dest) {
  int *sendBuff = malloc(nData*sizeof(int));
  int i;
  for(i = 0;i < nData;i++) {
    sendBuff[i] = opt->to_partition[lastTask[1] + i];
  }
  MPI_Send(sendBuff,nData,MPI_INT,dest,SEND_TASK,MPI_COMM_WORLD);
  free(sendBuff);
}

void sendQuit(int rank) {
  MPI_Send(NULL,0,MPI_INT,rank,TAG_QUIT,MPI_COMM_WORLD);
}

void rootStuff(options* opt){
  int rank = my_rank();
  int numProcs;
  MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
  // Determine number of tasks
  int numTasks = opt->num_to_partition/opt->chunksize;
  if (opt->num_to_partition % opt->chunksize != 0) {
    numTasks++;
  }
  // Keeping track of tasks assigned and completed as well as describing them
  // with as (id, lower index, upper index)
  int tasksAssigned = 0;
  int tasksCompleted = 0;
  int lastTask[3] = {-1,1,1};
  // Assign each worker an initial task unless there are none left to assign
  int i = 0;
  for(i = 1;i < numProcs;i++) {
    if (tasksAssigned < numTasks){ 
      lastTask[0]++;
      lastTask[1] = lastTask[2] + 1;
      lastTask[2] = min(lastTask[1]+opt->chunksize-1, opt->num_to_partition);
      int nData = lastTask[2] - lastTask[1] + 1;
      sendTask(lastTask,nData,opt,i);
      tasksAssigned++;
    }
    else {
      sendQuit(i);
    }
  }
  // Allocate remaining tasks
  while(tasksCompleted < numTasks) {
    MPI_Status status;
    // Receive request for new task
    int buff;
    MPI_Recv(&buff,1,MPI_INT,MPI_ANY_SOURCE,REQUEST_TASK,MPI_COMM_WORLD,&status);
    int workerID = status.MPI_SOURCE;
    tasksCompleted++;
    // Assign requesting core a new task if there exists remaining tasks
    if (tasksAssigned < numTasks) {
      lastTask[0]++;
      lastTask[1] = lastTask[2] + 1;
      lastTask[2] = min(lastTask[1]+opt->chunksize-1, opt->num_to_partition);
      int nData = lastTask[2] - lastTask[1] + 1;
      sendTask(lastTask,nData,opt,i);
      tasksAssigned++;
    }
    // Send quite message if no tasks remain
    else {
      sendQuit(workerID);
    }
  }
}

void workerStuff(options* opt){
  int rank = my_rank();
  MPI_Status status;
  int* recvBuff = malloc(opt->chunksize*sizeof(int));
  while (1) {
    MPI_Recv(recvBuff,opt->chunksize,MPI_INT,0,SEND_TASK,MPI_COMM_WORLD,&status);
    int nRecv = status.count;
    int i;
    int a;
    int b;
    int isGoldbach;
    for (i = 0;i < nRecv;i++) {
      isGoldback = goldbach_partition(recvBuff[i],&a,&b);
      if 
    }
  }
  free(recvBuff);
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

  //The command line options have now been parsed properly.
  //They are held in the members of opt.
  //The options you care about are:
  //opt.print_results  -- 0 if you should not print out the partitions you find, 1 if you should
  //opt.to_partition   -- the list of numbers that you should find partitions for
  //opt.num_to_partition -- the length of the list above
  //opt.chunksize -- the control for the "chunksize" of a work assignment.  If the chunksize is n, then processors get assigned to partition n consecutive numbers from the to_partition list as part of 1 assignement. 
  
  MPI_Finalize();
  return 0;
}
