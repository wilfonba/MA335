#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<string.h>

int __real_MPI_Send(void* buf, int count, MPI_Datatype datatype,int source,int tag,MPI_Comm comm);
int __real_MPI_Recv(void* buf, int count, MPI_Datatype datatype,int source,int tag,MPI_Comm comm, MPI_Status* stat);


extern int num_sends;
extern int send_records_len;
extern int** send_records;

extern int num_recvs;
extern int recv_records_len;
extern int** recv_records;

int __wrap_MPI_Recv(void* buf, int count, MPI_Datatype datatype,int source,int tag,MPI_Comm comm,MPI_Status* stat){
  if (num_recvs==recv_records_len){
    recv_records=realloc(recv_records,(2*recv_records_len+1)*sizeof(int*));
    recv_records_len=2*recv_records_len+1;
  }
  
  int rank;
  MPI_Comm_rank(comm,&rank);

  MPI_Status tmpstat;
  int R;
  R=__real_MPI_Recv(buf,count,datatype,source,tag,comm,&tmpstat);

  int L;
  MPI_Get_count(&tmpstat,datatype,&L);
  recv_records[num_recvs]=malloc((L+2)*sizeof(int));
  recv_records[num_recvs][0]=tmpstat.MPI_SOURCE;
  recv_records[num_recvs][1]=L;
  memcpy(recv_records[num_recvs]+2,buf,L*sizeof(int));
  num_recvs++;

  if (stat != MPI_STATUS_IGNORE){
    memcpy(stat,&tmpstat,sizeof(MPI_Status));
  }

  return R;

}



int __wrap_MPI_Send(void* buf, int count, MPI_Datatype datatype,int dest,int tag,MPI_Comm comm){
  if (num_sends==send_records_len){
    send_records=realloc(send_records,(2*send_records_len+1)*sizeof(int*));
    send_records_len=2*send_records_len+1;
  }
  
  int rank;
  MPI_Comm_rank(comm,&rank);
  
  send_records[num_sends]=malloc((count+2)*sizeof(int));
  send_records[num_sends][0]=dest;
  send_records[num_sends][1]=count;
  memcpy(send_records[num_sends]+2,buf,count*sizeof(int));
  num_sends++;
  
  return __real_MPI_Send(buf,count,datatype,dest,tag,comm);

}
