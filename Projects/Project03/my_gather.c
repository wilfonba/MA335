#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>

//#define DEBUG

void my_gather(int* recvbuf,int recvcount,int* sendbuf,int sendcount,int root)
{
    int size;
    int myRank;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
    int myLabel = (myRank - root + size)%size;
    int maxLabel = size - 1;
    int currBuffLeng = sendcount;
    int k = 1;
    int offset = 1;
    int* currBuff = malloc(currBuffLeng*sizeof(int));
    currBuff = memcpy(currBuff,sendbuf,sendcount*sizeof(int));
    int itr = 0;
    while (myLabel%2 == 0 && offset < size) {
        currBuffLeng*=2;
        currBuff = realloc(currBuff,currBuffLeng*sizeof(int));
        if (myLabel < maxLabel) {
            #ifdef DEBUG
                printf("Rank %d receiving from rank %d\n",myRank,(myRank + offset + size)%size);
            #endif
            MPI_Recv(currBuff + currBuffLeng/2,currBuffLeng/2,MPI_INT,(myRank + offset + size)%size,0,MPI_COMM_WORLD,NULL);
        }
        myLabel = myLabel/2;
        offset*=2;
        itr++;
        maxLabel = maxLabel/2;
        if (maxLabel == 0) {
            break;
        }
    }
    if (myLabel != 0) {
        #ifdef DEBUG
            printf("Rank %d sending to rank %d\n",myRank,(myRank - offset + size)%size);
        #endif
        MPI_Send(currBuff,currBuffLeng,MPI_INT,(myRank - offset + size)%size,0,MPI_COMM_WORLD);
    }
    else {
        if (root == 0) {
            recvbuf = memcpy(recvbuf,currBuff,size*sendcount*sizeof(int));
        }
        else {
            //recvbuf = memcpy(recvbuf,currBuff,size*sendcount*sizeof(int));
            recvbuf = memcpy(recvbuf,currBuff + (size - root)*sendcount,root*sendcount*sizeof(int));
            recvbuf = memcpy(recvbuf + root*sendcount,currBuff,(size - root)*sendcount*sizeof(int));
        }
    }
    free(currBuff);
    return;
}

int main(int* argv, char** argc) {
    MPI_Init(NULL,NULL);
    int rank;
    int size;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int data[4] = {0,1,2,3};
    if (rank == 0)
    {
        int* buff = malloc(20*sizeof(int));
        my_gather(buff,20,data,4,0);
    }
    else {
        my_gather(NULL,0,data,4,0);
    }

    MPI_Finalize();
    return 1;
}