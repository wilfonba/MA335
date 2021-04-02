#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<string.h>

int __real_MPI_Recv(void* buf, int count, MPI_Datatype datatype,int source,int tag,MPI_Comm comm,MPI_Status* status);
int __real_MPI_Send(void* buf, int count, MPI_Datatype datatype,int source,int tag,MPI_Comm comm);
int __real_MPI_Finalize();

int num_messages_recieved=0;
int* message_contents=NULL;
int* senders=NULL;
int* message_sizes_B=NULL;
int logsize=1000;

int __wrap_MPI_Recv(void* buf, int count, MPI_Datatype datatype,int source,int tag,MPI_Comm comm,MPI_Status* status){
  //int messagesize;
  int bytesperunit;
  MPI_Type_size(datatype,&bytesperunit);
  //messagesize=bytesperunit*count;
  int rank;
  MPI_Comm_rank(comm,&rank);
  num_messages_recieved++;
  if (message_contents==NULL){
    message_contents=malloc(logsize*sizeof(int));
  }
  if (senders==NULL){
    senders=malloc(logsize*sizeof(int));
  }
  if (message_sizes_B==NULL){
    message_sizes_B=malloc(logsize*sizeof(int));
  }
  int stat;
  MPI_Status tmpstatus;
  stat=__real_MPI_Recv(buf,count,datatype,source,tag,comm,&tmpstatus);
  message_contents[num_messages_recieved-1]=((int*)buf)[0];
  senders[num_messages_recieved-1]=tmpstatus.MPI_SOURCE;

  int tmp;
  MPI_Get_count(&tmpstatus,datatype,&tmp);
  message_sizes_B[num_messages_recieved-1]=tmp*bytesperunit;

  if (status != MPI_STATUS_IGNORE){
    memcpy(status,&tmpstatus,sizeof(MPI_Status));
  }
  
  return stat;

}

int __wrap_MPI_Finalize(){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  char filename[20];
  sprintf(filename,"whatidid.rank.%d",rank);
  FILE* f = fopen(filename,"w");
  int i;
  fprintf(f,"Recv. No.\tSender\tData(int)\tData Size (B)\n");
  for (i=0;i<num_messages_recieved;i++){
    fprintf(f,"%d\t%d\t%d\t%d\n",i,senders[i],message_contents[i],message_sizes_B[i]);
  }
  fclose(f);
  return __real_MPI_Finalize();

}
