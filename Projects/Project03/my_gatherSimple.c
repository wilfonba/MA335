#include <mpi.h>
#include <stdio.h>
#include "arr2str.c"


void my_gather(int* sendbuf, int len,int* recvbuf,int root){
	//Let's assume that the root is rank 0, and abort otherwise. 
	if (root!=0){
		fprintf(stderr,"Error, my_gather is only written for the case when the root is rank 0.\n");
		MPI_Abort(MPI_COMM_WORLD,1);
	}

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD,&size);

  //Let's assume that the number of processors in existence is a power of 2, and
  //quit otherwise.  There are clever ways to do this -- but let's avoid that. 
	int k=0;
	int two_to_k=1;
	while (two_to_k<size){
		k=k+1;
		two_to_k=two_to_k*2;
	}
	if (size!=two_to_k){
		fprintf(stderr,"Error, my_gather only works when there are a power of two number of processors running.\n");
		MPI_Abort(MPI_COMM_WORLD,1);
	}

	//This is the "label" that we use to decide whether to send or recieve. 
	int my_label=rank;
  //This is the offset that tells us how far away the processor is that we are
  //send/recieving from.
	int offset=1;
  //This is just a counter for convenience with output -- not really needed.
  //Tells us what "wave" we are on.
	int iteration=0;
  //If we have to do any recieving, keep a buffer in which to store messages.
  //Copy in the message you wish to send. 
	int cur_buffer_len=len;
	int* cur_buffer=malloc(cur_buffer_len*sizeof(int));
	memcpy(cur_buffer,sendbuf,len*sizeof(int));

	while (my_label % 2 ==0 && offset < size){
		printf("Iter: %d Rank: %d Rcv from: %d\n",iteration,rank,rank+offset);
		//Grow the send/recieve buffer to accomodate the incoming message. 
		cur_buffer_len=cur_buffer_len*2;
		cur_buffer=realloc(cur_buffer,cur_buffer_len*sizeof(int));
    //Do your recieve.  Stick the message halfway out in the cur_buffer, because
    //your existing data lives in the first half of it. 
		MPI_Recv(cur_buffer+cur_buffer_len/2,cur_buffer_len/2,MPI_INT,rank+offset,0,
             MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		//Print your state.
		printf("Iter: %d Rank: %d Recv complete new buffer contents: %s\n",iteration,rank,int_arr2str(cur_buffer,cur_buffer_len));
		my_label=my_label/2;
		offset=offset*2;
		iteration++;

	}
	if (rank != 0) {
		printf("Iter: %d Rank: %d Send to: %d\n",iteration,rank,rank-offset);
		MPI_Send(cur_buffer,cur_buffer_len,MPI_INT,rank-offset,0,MPI_COMM_WORLD);
	}

	if (rank == 0) {
		memcpy(recvbuf,cur_buffer,cur_buffer_len*sizeof(int));
	}
	free(cur_buffer);

}
