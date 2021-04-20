#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

#include "arr2str.c"

int main(int argc, char** argv){
  MPI_Init(&argc,&argv);
  int total_procs;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD,&total_procs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  //should we put the barrier in place to make sure that 
  //rank 0 gets a chance to print out it's rank first?
  int do_barrier=0;
  if (argc>1) {
    sscanf(argv[1],"%d",&do_barrier);
  }
 
  //Print out the options.
  if (rank==0) {
    printf("Demonstrating MPI_Barrier.\n");
    if (do_barrier) {
      printf("Use Barrier: yes\n");
    } else {
      printf("Use barrier: no\n");
    }
    fflush(stdout);
  }

  //Suppose we want everyone to print out their rank, but that rank 0 has some work to do first. 
  //In this case, we want it to sleep for 5 seconds.  No one else has to sleep, so they will
  //certianly get to print thier rank first. 

  //UNLESS, we make everyeone wait until rank 0 has had a chance to print his rank, by using an MPI
  //barrier. 

  //If you are rank 0, do your work (sleep), print your message, then check in with the barrier 
  //if we are using it. 
  if (rank==0) {
    sleep(5);
    printf("Hi there, I'm rank 0, and I'm finally done with my work. \n");
    fflush(stdout);
    if (do_barrier) {
      MPI_Barrier(MPI_COMM_WORLD);
    }
  } else {
    //All non-rank 0 process just print their rank. They will certainly beat rank 0 to that, 
    //UNLESS they have to wait for rank 0 as part of the barrier call. 
    if (do_barrier) {
      MPI_Barrier(MPI_COMM_WORLD);
    }
    printf("Rank %d checking in.\n",rank);
  }

  MPI_Finalize();

}
