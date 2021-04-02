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

  if (argc<2){
    rootprint("Usage: %s T\n",argv[0]);
    rootprint("where T is the number of trials on each processor.\n");
    MPI_Finalize();
    return(MPI_SUCCESS);
  }

  //You fill in this part. 


  
  MPI_Finalize();
  return(MPI_SUCCESS);
}
