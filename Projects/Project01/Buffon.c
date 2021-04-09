#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
#include<math.h>
#include<unistd.h>

#define rootprint(...) rank==0 ? printf(__VA_ARGS__):0 ; fflush(stdout);

int crosses_line_calls=0;

int crosses_line(double x_end, double y_end, double theta){
	crosses_line_calls++;
  if (y_end+sin(theta)>1){
    return(1);
  }
  else{
    return(0);
  }

}  

int main(int argc, char** argv){
  MPI_Init(&argc,&argv);

  int rank;
  int numprocs;
  int i;
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  int T;
  sscanf(argv[1],"%d",&T);

  if (argc<2){
    rootprint("Usage: %s T\n",argv[0]);
    rootprint("where T is the number of trials on each processor.\n");
    MPI_Finalize();
    return(MPI_SUCCESS);
  }

  double x_end;
  double y_end;
  double theta;
  double numCross = 0;
  srand(time(NULL) + 100*rank);
  for (i;i < T;i++) {
    x_end = 1.0*rand()/RAND_MAX;
    y_end = 1.0*rand()/RAND_MAX;
    theta = 2*3.1415926*rand()/RAND_MAX;
    if (crosses_line(x_end,y_end,theta))
    {
      numCross++;
    } 
  }

  double myPi;
  myPi = T/numCross;
  double pisTotal;
  double piEst;
  if (rank > 0) {
    MPI_Send(&myPi,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
  }
  else if (rank == 0) {
    pisTotal = myPi;
    for (i = 1;i < numprocs;i++)
    {
      double buff;
      MPI_Recv(&buff,1,MPI_DOUBLE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,NULL);
      pisTotal += buff;
    }
    piEst = pisTotal/numprocs;
    printf("\n\n Pi is approximately %lf \n\n", piEst);
  }
  
  MPI_Finalize();
  return(MPI_SUCCESS);
}
