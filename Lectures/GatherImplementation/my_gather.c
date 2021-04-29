#include <mpi.h>
#include <stdio.h>
#include "arr2str.c"
#include <math.h>

void my_gather(int* sendbuf, int len, int* recvbuf, int root) {
	int rank;
	int size;

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	int myRank = rank;

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
		int N = (int) len*pow(2,level-1); 
		if (myRank % 2 == 0) {
			/*tempBuff = realloc(tempBuff,N*sizeof(int));
			MPI_Recv(tempBuff,N,MPI_INT,0,myRank + 1,MPI_COMM_WORLD,NULL);
			printf("Rank %d received [%d,%d,%d,%d]\n",myRank,tempBuff[0],tempBuff[1],tempBuff[2],tempBuff[3]);
			fflush(stdout);
			memcpy(recvBuff+N,tempBuff,len*pow(2,level-1)*sizeof(int));
			myRank = myRank/2;
			if (myRank % 2 != 0) {
				tempBuff = realloc(tempBuff,2*N*sizeof(int));
				memcpy(sendBuff,recvBuff,2*N*sizeof(int));
			}*/
		}
		else {
			printf("N = %d: Rank %d sending [%d,%d,%d,%d]\n",N,myRank,sendBuff[0],sendBuff[1],sendBuff[2],sendBuff[3]);
			fflush(stdout);
			MPI_Send(sendBuff,N,MPI_INT,0,myRank - 1,MPI_COMM_WORLD);
			myRank = myRank/2;
		}

		level++;
	}
}
