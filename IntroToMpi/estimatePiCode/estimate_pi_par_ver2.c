#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<mpi.h>


//This is a program that estimates pi by throwing random darts at a 1x1
//dartboard, and recording how many fall within 1 quadrant of the unit circle. 


//To use the program, do 
//./estimate_pi_serial <number of darts to throw> 
//if <number of darts to throw is not given on the command line, the default is
//1000. 
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
    //If we did rand()/RAND_MAX that would do integer division, which is not
    //what we want. So:
    x=1.0*rand()/RAND_MAX;
    y=1.0*rand()/RAND_MAX;
    //see if they happen to be in unit circle
    if (x*x+y*y < 1) {
      //record success
      successes+=1;
    }
  }
  //A random dart should land in that circle with probability pi/4.  So, we can
  //estmate pi as //pi_est =4*successes/numtrials
  //watch out!  C does integer division by default, so include a double here to
  //make it realize that I want a double division. 
  double pi_est=4.0*successes/N;

  //All ranks other than 0 just send their estimate of pi back to the rank 0
  //processor. 
  if (my_rank!=0) {
    //printf("Rank: %d Pi Estimate: %lf Trials: %d sending back to rank 0\n",my_rank,pi_est,N);

    //MPI_Send(void* data,int count,MPI_Datatype datatype,int destination,int tag,MPI_Comm communicator)
    MPI_Send(&pi_est,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
  }

  //The rank 0 processor must recieve the estimate of pi from each worker, and
  //then averages the results. 
  if (my_rank==0) {
    double small_pi_estimates[num_procs];
    //small_pi_estimates[j] will contain the estimate of pi from rank j. 
    small_pi_estimates[0]=pi_est;

    //the buffer where we will use for receiving. 
    double dummy;

    //The MPI_Status struct will contain information about the message after it is recieved. 
    MPI_Status stat;

    int messg_source;
    //If there are num_procs processes then there are num_procs-1 workers. Since
    //each MPI_Recv will recieve one message, we must call MPI_Recv num_procs-1
    //times. 
    for (i=0;i<num_procs-1;i++) {
      //MPI_Recv(void* data,int count,MPI_Datatype datatype,int source,
      //         int tag,MPI_Comm communicator, MPI_Status* status)
      //Recieve a message from anyone, with any tag. Put it in the dummy buffer,
      //which is long enough for one double. 
      MPI_Recv(&dummy,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&stat);
      //Figure out who the message came from. 
      messg_source=stat.MPI_SOURCE;
      //Store the message contents in the small_pi_estimates array. 
      small_pi_estimates[messg_source]=dummy;
      //printf("Rank %d estimate of pi: %lf\n",messg_source,small_pi_estimates[messg_source]);
    }

    double total=0;
    //Now average the results. 
    for (i=0;i<num_procs;i++) {
      total+=small_pi_estimates[i];
    }
    double final_estimate=total/num_procs;
    printf("Rank 0 Samples: %d Estimate of Pi: %lf\n",num_procs*N,final_estimate);

  }

  MPI_Finalize();
  return(0);
}
