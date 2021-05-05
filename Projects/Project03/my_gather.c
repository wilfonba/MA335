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
    int myLabel = (myRank - root + size)%size;
    int maxLabel = size - 1;
    int currBuffLeng = sendcount;
    int k = 1;
    int offset = 1;
    int* currBuff = malloc(currBuffLeng*sizeof(int));
    currBuff = memcpy(currBuff,sendbuf,sendcount*sizeof(int));
    int itr = 0;
    while ((myLabel%2 == 0) || (myLabel == maxLabel)) {
        if (myLabel == maxLabel && maxLabel%2 != 0) {
            printf("MAX LABEL = %d and I'm rank %d with label %d trying to send to %d of length %d\n\n",maxLabel,
                    myRank,myLabel,(myRank - offset + size)%size,currBuffLeng/(int)pow(2,itr));
            MPI_Send(currBuff,currBuffLeng,MPI_INT,(myRank - offset + size)%size,0,MPI_COMM_WORLD);
            if (maxLabel == 1) {
                break;
            }
        }
        else if (myLabel == 0 && maxLabel%2 != 0) {
            printf("MAX LABEL = %d and I'm rank %d with label %d trying to send to %d of length %d\n\n",maxLabel,
                    myRank,myLabel,(myRank - offset + size)%size,currBuffLeng/(int)pow(2,itr));
            int toSend = currBuffLeng/(int)pow(2,itr);
            currBuffLeng += toSend;
            currBuff = realloc(currBuff,currBuffLeng*sizeof(int));
            MPI_Recv(currBuff + currBuffLeng - toSend,toSend,MPI_INT,(myRank + offset + size)%size,0,MPI_COMM_WORLD,NULL);
        }
        currBuffLeng*=2;
        currBuff = realloc(currBuff,currBuffLeng*sizeof(int));
        if (myLabel != maxLabel) {
            printf("\n      At itr %d rank %d (label %d) receiving from rank %d, MAX LABAEL = %d, buff leng = \n",
                    itr,myRank,myLabel,(myRank+offset+size)%size,maxLabel,currBuffLeng);
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
        MPI_Send(currBuff,currBuffLeng,MPI_INT,(myRank - offset + size)%size,0,MPI_COMM_WORLD);
    }
    else if (myLabel == 0) {
        if (root == 0) {
            recvbuf = memcpy(recvbuf,currBuff,size*sendcount*sizeof(int));
        }
        else {
            //recvbuf = memcpy(recvbuf,currBuff + (root+1)*sendcount,root*sendcount*sizeof(int));
            //recvbuf = memcpy(recvbuf + root*sendcount,currBuff,(root+1)*sendcount*sizeof(int));
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