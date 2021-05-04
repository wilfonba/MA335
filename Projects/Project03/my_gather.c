#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>

void my_gather(int* recvbuf,int recvcount,int* sendbuf,int sendcount,int root)
{
    int size;
    int myRank;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
    
    int myLabel = myRank;
    int maxLabel = size - 1;
    int* tempRecv = malloc(recvcount*sizeof(int));
    int* tempSend = malloc(sendcount*sizeof(int));
    printf("Rank %d has myLabel = %d and maxLabel = %d\n",myRank,myLabel,maxLabel);
    fflush(stdout);
    int k = 1;
    while(1) {

    }
}