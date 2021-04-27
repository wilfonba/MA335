#include <mpi.h>
#include <stdio.h>
#include "arr2str.c"

void my_gather(int* sendbuf, int len, int* recvbuf, int root) {
	int rank;
	int size;

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	//Let's assume that the root is rank 0, and abort otherwise. 
	if (root!=0){
		fprintf(stderr,"Error, my_gather is only written for the case when the root is rank 0.\n");
		MPI_Abort(MPI_COMM_WORLD,1);
	}
	if (size % 2 != 0){
		fprintf(stderr,"Error, my gather is only written for the case where there is an even number of processors.\n");
	}

	int level = 1;
	int* sendBuff = malloc(len*sizeof(int));
	memcpy(sendBuff,sendbuf,len*sizeof(int));

	int* recvBuff = malloc(len*sizeof(int));
	memcpy(recvBuff,recvbuf,len*sizeof(int));

	int* tempBuff = malloc(len*sizeof(int));
	while (level < size/2) {
		if (rank % level*2 == 0) { 
			MPI_Recv(tempBuff,len*level,MPI_INT,rank+level,0,MPI_COMM_WORLD,NULL);
			recvBuff = realloc(recvBuff,2*sizeof(*recvBuff));
			memcpy(recvBuff,tempBuff,sizeof(*tempBuff));
			if (rank % level*4 != 0){
				sendBuff = realloc(sendBuff,2*sizeof(*sendBuff));
				memcpy(sendBuff,tempBuff,sizeof(sendBuff));
				tempBuff = realloc(tempBuff,2*sizeof(*tempBuff));	
			}
		}
		else {
			MPI_Send(sendBuff,len*level,MPI_INT,rank,0,MPI_COMM_WORLD);
		}
		level*=2;
	}
}
