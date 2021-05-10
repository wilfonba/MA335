#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>

extern long long int set_state_calls;

int __real_MPI_Init(int* argc, char*** argv);
int __real_MPI_Finalize();
int __real_MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,MPI_Comm comm);

int __real_MPI_Recv(void *buf, int count, MPI_Datatype datatype,int source, int tag, MPI_Comm comm, MPI_Status *status);

int __real_MPI_Alltoall(const void *sendbuf, int sendcount,MPI_Datatype sendtype, void *recvbuf, int recvcount,MPI_Datatype recvtype, MPI_Comm comm);


int __real_MPI_Scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,MPI_Comm comm);

int __real_MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, int recvcount, MPI_Datatype recvtype,int root, MPI_Comm comm);
  
int __real_MPI_Allgather(const void *sendbuf, int  sendcount,MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

int __real_MPI_Alltoallv(const void *sendbuf, const int *sendcounts, const int *sdispls, MPI_Datatype sendtype, void *recvbuf,const int *recvcounts, const int *rdispls, MPI_Datatype recvtype,MPI_Comm comm);

int __real_MPI_Allgatherv(const void *sendbuf, int sendcount,MPI_Datatype sendtype, void *recvbuf, const int recvcounts[],const int displs[], MPI_Datatype recvtype, MPI_Comm comm);
  
int __real_MPI_Scatterv(const void *sendbuf, const int sendcounts[], const int displs[],MPI_Datatype sendtype, void *recvbuf, int recvcount,MPI_Datatype recvtype, int root, MPI_Comm comm);


int __real_MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype,int root, MPI_Comm comm);  

int __real_MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,MPI_Comm comm, MPI_Request *request);

int __real_MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,int dest, int sendtag, void *recvbuf, int recvcount,MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);


double g_start_time;
double g_end_time;
int num_misc_sends=0;
int num_alltoallv_sends=0;
int num_allgatherv_sends=0;
int num_scatterv_sends=0;
int num_gatherv_sends=0;
int num_sendrecv_sends=0;

int __wrap_MPI_Init(int* argc, char*** argv){
  int s;
  s=__real_MPI_Init(argc,argv);
  g_start_time=MPI_Wtime();
  return s;
}

int __wrap_MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,MPI_Comm comm){
  num_misc_sends+=count;
  return __real_MPI_Send(buf,count,datatype,dest,tag,comm);
}


int __wrap_MPI_Alltoall(const void *sendbuf, int sendcount,MPI_Datatype sendtype, void *recvbuf, int recvcount,MPI_Datatype recvtype, MPI_Comm comm){
  num_misc_sends+=sendcount;
  return __real_MPI_Alltoall(sendbuf,sendcount,sendtype,recvbuf,recvcount,recvtype,comm);
}


int __wrap_MPI_Scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,MPI_Comm comm){
  if (sendbuf!=NULL){
    num_misc_sends+=sendcount;
  }
  return __real_MPI_Scatter(sendbuf,sendcount,sendtype,recvbuf,recvcount,recvtype,root,comm);
}

int __wrap_MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, int recvcount, MPI_Datatype recvtype,int root, MPI_Comm comm){
  num_misc_sends+=sendcount;
  return __real_MPI_Gather(sendbuf,sendcount,sendtype,recvbuf,recvcount,recvtype,root,comm);
}
  
int __wrap_MPI_Allgather(const void *sendbuf, int  sendcount,MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm){
  num_misc_sends+=sendcount;
  return __real_MPI_Allgather(sendbuf,sendcount,sendtype,recvbuf,recvcount,recvtype,comm);
}

int __wrap_MPI_Alltoallv(const void *sendbuf, const int *sendcounts, const int *sdispls, MPI_Datatype sendtype, void *recvbuf,const int *recvcounts, const int *rdispls, MPI_Datatype recvtype,MPI_Comm comm){
  int size;
  MPI_Comm_size(comm,&size);
  int i;
  for (i=0;i<size;i++){
    num_misc_sends+=sendcounts[i];
  }
  return __real_MPI_Alltoallv(sendbuf,sendcounts,sdispls,sendtype,recvbuf,recvcounts,rdispls,recvtype,comm);
}

int __wrap_MPI_Allgatherv(const void *sendbuf, int sendcount,MPI_Datatype sendtype, void *recvbuf, const int recvcounts[],const int displs[], MPI_Datatype recvtype, MPI_Comm comm){
  num_misc_sends+=sendcount;
  return __real_MPI_Allgatherv(sendbuf,sendcount,sendtype,recvbuf,recvcounts,displs,recvtype,comm);
}
  
int __wrap_MPI_Scatterv(const void *sendbuf, const int sendcounts[], const int displs[],MPI_Datatype sendtype, void
			*recvbuf, int recvcount,MPI_Datatype recvtype, int root, MPI_Comm comm){

  int size,i;
  if (sendbuf!=NULL){
    MPI_Comm_size(comm,&size);
    for (i=0;i<size;i++){
      num_misc_sends+=sendcounts[i];
    }
  }
  return __real_MPI_Scatterv(sendbuf,sendcounts,displs,sendtype,recvbuf,recvcount,recvtype,root,comm);
}


int __wrap_MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype,int root, MPI_Comm comm){

  num_misc_sends+=sendcount;
  return __real_MPI_Gatherv(sendbuf,sendcount,sendtype,recvbuf,recvcounts,displs,recvtype,root,comm);

}

int __wrap_MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,MPI_Comm comm, MPI_Request *request){
  num_misc_sends+=count;
  return __real_MPI_Isend(buf,count,datatype,dest,tag,comm,request);

}


int __wrap_MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,int dest, int sendtag, void *recvbuf, int recvcount,MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status){
  num_sendrecv_sends+=sendcount;
  return __real_MPI_Sendrecv(sendbuf,sendcount,sendtype,dest,sendtag,recvbuf,recvcount,recvtype,source,recvtag,comm,status);
}

int __wrap_MPI_Finalize(){
  g_end_time=MPI_Wtime();
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

 
  char outputfile[1000];
  sprintf(outputfile,"whatidid.rank.%d.txt",rank);
  FILE* f=fopen(outputfile,"w");
  fprintf(f,"ET: %lf\n",g_end_time-g_start_time);
  fprintf(f,"Misc sends: %d\n",num_misc_sends);
  fprintf(f,"Sendrecv sends: %d\n",num_sendrecv_sends);
  fprintf(f,"set_state calls: %lld\n",set_state_calls);
  
  fclose(f);
  
  return __real_MPI_Finalize();
}
