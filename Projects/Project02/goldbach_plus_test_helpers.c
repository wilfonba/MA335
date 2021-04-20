#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int __real_MPI_Init(int*,char***);
int __real_MPI_Finalize();
int __real_MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,
		    MPI_Comm comm);
  
extern int goldbach_partition_calls;
int num_sends=0;
int send_data_count=0;
double global_start_time;
double global_end_time;


int __wrap_MPI_Init(int* argc, char*** argv){
  int s;
  s=__real_MPI_Init(argc,argv);
  global_start_time=MPI_Wtime();
  return s;
}

int __wrap_MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,
		    MPI_Comm comm){
  num_sends++;
  send_data_count+=count;
  return __real_MPI_Send(buf,count,datatype,dest,tag,comm);
}

int __wrap_MPI_Finalize(){
  global_end_time=MPI_Wtime();
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  char filename[1000];
  sprintf(filename,"whatidid.rank.%d",rank);
  FILE* f=fopen(filename,"w");
  fprintf(f,"ET: %lf\n",global_end_time-global_start_time);
  fprintf(f,"Num Sends: %d\n",num_sends);
  fprintf(f,"Num partition calls: %d\n",goldbach_partition_calls);
  fprintf(f,"Total number of data send: %d\n",send_data_count);
  fclose(f);
  return __real_MPI_Finalize();
}

