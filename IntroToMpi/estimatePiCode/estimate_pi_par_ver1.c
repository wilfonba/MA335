#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<mpi.h>


//This is a program that estimates pi by throwing random darts at a 1x1 dartboard, and recording how many fall within 
//1 quadrant of the unit circle. 


//To use the program, do 
//./estimate_pi_serial <number of darts to throw> 
//if <number of darts to throw is not given on the command line, the default is 1000. 
int main(int argc, char** argv) {
  MPI_Init(&argc,&argv);

  int num_procs;
  int my_rank;
  // num_procs says how many processes are running this time.
  MPI_Comm_size(MPI_COMM_WORLD,&num_procs); 
  //rank holds the rank of the currently running process. 
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);  

 
  int N;
  //test to see if the user gave us an extra command line argument for number of
  //trials. 
  if (argc<2) {
    N=1e3;
  } else {
    N=atoi(argv[1]);
  }

  //seed the random number generator 
  //any time you want random numbers you must seed the random number generator. 
  int basetime=time(NULL);
  srand(basetime+100*my_rank);

  //Throw a dart at the square [0,1] x [0,1] numtrials times.  Record how many
  //times the dart lands inside the unit circle. 
  double x,y;
  int successes=0;
  int i;
  for (i=0;i<N;i++) {
    //generate random coordinates.
    //rand() returns an integer between 0 and RAND_MAX
    //If we did rand()/RAND_MAX that would do integer division so 1.0 makes a
    //double
    x=1.0*rand()/RAND_MAX;
    y=1.0*rand()/RAND_MAX;
    //see if they happen to be in unit circle
    if (x*x+y*y < 1) {
      //record success
      successes+=1;
    }
  }
  //A random dart should land in that circle with probability pi/4.
  //So, pi_est =4*successes/numtrials
  double pi_est=4.0*successes/N;
  printf("Rank: %d number of samples: %d estimate: %lf\n",my_rank,N,pi_est);
    
  if (my_rank!=0) {
    MPI_Send(&pi_est,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
  }  

  MPI_Finalize();
  return(0);
}
